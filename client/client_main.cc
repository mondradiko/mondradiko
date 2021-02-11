// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <csignal>
#include <iostream>
#include <sstream>

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

// The using statement is fine because
// this is the main entrypoint
using namespace mondradiko;  // NOLINT

bool g_interrupted = false;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;

  g_interrupted = true;

  return;
}

void session_loop(Filesystem* fs, DisplayInterface* display, GpuInstance* gpu) {
  if (!display->createSession(gpu)) {
    log_ftl("Failed to create display session!");
  }

  auto config = fs->loadToml("./config.toml");

  CVarScope cvars;
  GlyphLoader::initCVars(&cvars);
  Renderer::initCVars(&cvars);
  NetworkClient::initCVars(&cvars);
  cvars.loadConfig(config);

  GlyphLoader glyphs(&cvars, gpu);
  World world(fs, gpu);

  Renderer renderer(&cvars, display, gpu);
  MeshPass mesh_pass(&renderer, &world);
  OverlayPass overlay_pass(cvars.getChild("renderer"), &glyphs, &renderer,
                           &world);

  renderer.addRenderPass(&mesh_pass);
  renderer.addRenderPass(&overlay_pass);

  UserInterface ui(&glyphs, &renderer);
  renderer.addRenderPass(&ui);

  NetworkClient client(&cvars, fs, &world, "127.0.0.1", 10555);

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

void player_session_run(const char* server_address, int port) {
  Filesystem fs;
  fs.loadAssetBundle("../test-folder/");

  OpenXrDisplay display;
  GpuInstance gpu(&display);

  session_loop(&fs, &display, &gpu);
}

void spectator_session_run(const char* server_address, int port) {
  Filesystem fs;
  fs.loadAssetBundle("./");

  SdlDisplay display;
  GpuInstance gpu(&display);

  session_loop(&fs, &display, &gpu);
}

int main(int argc, char* argv[]) {
  log_inf("Hello VR!");

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  try {
    // player_session_run("127.0.0.1", 10555);
    spectator_session_run("127.0.0.1", 10555);
  } catch (const std::exception& e) {
    log_err("Mondradiko player session failed with message:");
    log_err(e.what());
    return 1;
  }

  return 0;
}
