// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <csignal>
#include <iostream>
#include <memory>
#include <sstream>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "core/assets/AssetPool.h"
#include "core/cvars/CVarScope.h"
#include "core/displays/OpenXrDisplay.h"
#include "core/displays/SdlDisplay.h"
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "core/network/NetworkClient.h"
#include "core/renderer/MeshPass.h"
#include "core/renderer/OverlayPass.h"
#include "core/renderer/Renderer.h"
#include "core/ui/GlyphLoader.h"
#include "core/ui/UserInterface.h"
#include "core/world/World.h"
#include "log/log.h"
#include "types/build_config.h"

// The using statement is fine because
// this is the main entrypoint
using namespace mondradiko;  // NOLINT

struct ClientArgs {
  bool version = false;

  std::string server_ip = "127.0.0.1";
  int server_port = 10555;

  std::vector<std::string> bundle_paths = {"./"};

  std::string config_path = "./config.toml";

  int parse(int, char*[]);
};

int ClientArgs::parse(int argc, char* argv[]) {
  CLI::App app("Mondradiko client");

  app.add_flag("-v,--version", version, "Print version and exit");
  app.add_option("-s,--server", server_ip, "Domain server IP", true);
  app.add_option("-p,--port", server_port, "Domain server port", true);
  app.add_option("-b,--bundle", bundle_paths, "Paths to asset bundles", true);
  app.add_option("-c,--config", config_path, "Path to config file", true);

  CLI11_PARSE(app, argc, argv);
  return -1;
}

bool g_interrupted = false;

void run(const ClientArgs& args) {
  Filesystem fs;
  auto config = fs.loadToml(args.config_path);

  CVarScope cvars;
  GlyphLoader::initCVars(&cvars);
  Renderer::initCVars(&cvars);
  NetworkClient::initCVars(&cvars);
  cvars.loadConfig(config);

  for (auto bundle : args.bundle_paths) {
    fs.loadAssetBundle(bundle);
  }

  std::unique_ptr<DisplayInterface> display;
  display = std::make_unique<SdlDisplay>();

  GpuInstance gpu(display.get());
  if (!display->createSession(&gpu)) {
    log_ftl("Failed to create display session!");
  }

  AssetPool asset_pool(&fs);
  ScriptEnvironment scripts;
  World world(&asset_pool, &fs, &scripts);

  Renderer renderer(&cvars, display.get(), &gpu);
  GlyphLoader glyphs(&cvars, &renderer);
  MeshPass mesh_pass(&renderer, &world);
  OverlayPass overlay_pass(cvars.getChild("renderer"), &glyphs, &renderer,
                           &world);

  renderer.addRenderPass(&mesh_pass);
  renderer.addRenderPass(&overlay_pass);

  UserInterface ui(&glyphs, &renderer);
  renderer.addRenderPass(&ui);

  NetworkClient client(&cvars, &fs, &world, args.server_ip.c_str(),
                       args.server_port);

  while (!g_interrupted) {
    DisplayPollEventsInfo poll_info;
    poll_info.renderer = &renderer;

    display->pollEvents(&poll_info);
    if (poll_info.should_quit) break;

    if (poll_info.should_run) {
      DisplayBeginFrameInfo frame_info;
      display->beginFrame(&frame_info);

      if (!world.update()) break;

      if (frame_info.should_render) {
        renderer.renderFrame();
      }

      display->endFrame(&frame_info);
    }

    client.update();
  }

  renderer.destroyFrameData();
  display->destroySession();
}

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  g_interrupted = true;
  return;
}

int main(int argc, char* argv[]) {
  ClientArgs args;
  int parse_result = args.parse(argc, argv);
  if (parse_result != -1) return parse_result;

  log_msg_fmt("%s client version %s", MONDRADIKO_NAME, MONDRADIKO_VERSION);
  log_msg_fmt("%s", MONDRADIKO_COPYRIGHT);
  log_msg_fmt("%s", MONDRADIKO_LICENSE);

  if (args.version) return 0;

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  try {
    run(args);
  } catch (const std::exception& e) {
    log_err_fmt("Mondradiko player session failed with message: %s", e.what());
    return 1;
  }

  return 0;
}
