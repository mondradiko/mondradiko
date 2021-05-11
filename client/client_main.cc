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
using namespace mondradiko::core;  // NOLINT

struct ClientArgs {
  bool version = false;

  bool serverless = false;

  std::string server_ip = "127.0.0.1";
  int server_port = 10555;

  std::string display = "sdl";

  std::vector<std::string> bundle_paths = {"./"};

  std::string config_path = "./config.toml";

  int parse(int, const char* const[]);
};

int ClientArgs::parse(int argc, const char* const argv[]) {
  CLI::App app("Mondradiko client");

  app.add_flag("-v,--version", version, "Print version and exit");

  CLI::Option* serverless_op =
      app.add_flag("--serverless", serverless, "Run a serverless world");

  CLI::Option* server_op =
      app.add_option("-s,--server", server_ip, "Domain server IP", true);
  server_op->excludes(serverless_op);

  CLI::Option* port_op =
      app.add_option("-p,--port", server_port, "Domain server port", true);
  port_op->needs(server_op);

  app.add_option("-d,--display", display, "Display to use {sdl, openxr}", true);

  app.add_option("-b,--bundle", bundle_paths, "Paths to asset bundles", true);
  app.add_option("-c,--config", config_path, "Path to config file", true);

  CLI11_PARSE(app, argc, argv);
  return -1;
}

bool g_interrupted = false;

void run(const ClientArgs& args) {
  Filesystem fs;
  CVarScope cvars;

  GlyphLoader::initCVars(&cvars);
  GpuInstance::initCVars(&cvars);
  Renderer::initCVars(&cvars);
  NetworkClient::initCVars(&cvars);
  UserInterface::initCVars(&cvars);
  Display::initCVars(&cvars);
  cvars.loadConfigFromFile(&fs, args.config_path);

  for (auto bundle : args.bundle_paths) {
    fs.loadAssetBundle(bundle);
  }

  std::unique_ptr<Display> display;

  if (args.display == "sdl") {
    display = std::make_unique<SdlDisplay>(&cvars);
  } else if (args.display == "openxr") {
    display = std::make_unique<OpenXrDisplay>(&cvars);
  } else {
    log_ftl_fmt("Unrecognized display \"%s\" (must be {sdl, openxr})",
                args.display.c_str());
  }

  GpuInstance gpu(&cvars, display.get());
  if (!display->createSession(&gpu)) {
    log_ftl("Failed to create display session!");
  }

  AssetPool asset_pool(&fs);

  // TODO(marceline-cramer) Serverless world scripts
  World world(&asset_pool, &fs);

  Renderer renderer(&cvars, display.get(), &gpu);
  GlyphLoader glyphs(&cvars, &renderer);
  MeshPass mesh_pass(&renderer, &world);
  OverlayPass overlay_pass(cvars.getChild("renderer"), &glyphs, &renderer,
                           &world);

  renderer.addRenderPass(&mesh_pass);
  renderer.addRenderPass(&overlay_pass);

  UserInterface ui(&cvars, &fs, &glyphs, &renderer, &world);
  renderer.addRenderPass(&ui);
  display->setUserInterface(&ui);

  std::unique_ptr<NetworkClient> client;

  const Avatar* avatar = display->getAvatar(&world);

  if (!args.serverless) {
    client = std::make_unique<NetworkClient>(&cvars, &fs, &ui, &world);
    client->connect(avatar, args.server_ip.c_str(), args.server_port);
  } else {
    world.initializePrefabs();
    ui.displayMessage("Running serverless client");
  }

  while (!g_interrupted) {
    Display::PollEventsInfo poll_info;
    poll_info.renderer = &renderer;

    display->pollEvents(&poll_info);
    if (poll_info.should_quit) break;

    if (poll_info.should_run) {
      Display::BeginFrameInfo frame_info;
      display->beginFrame(&frame_info);

      if (!world.update(frame_info.dt)) break;

      if (!ui.update(frame_info.dt, overlay_pass.getDebugDraw())) break;

      if (frame_info.should_render) {
        renderer.renderFrame();
      }

      display->endFrame(&frame_info);
    }

    if (client) client->update();
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
