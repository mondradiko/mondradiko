/**
 * @file server_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Entrypoint for a server.
 * @date 2020-12-06
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include <csignal>
#include <iostream>

#include "core/assets/AssetPool.h"
#include "core/common/api_headers.h"
#include "core/displays/SdlDisplay.h"
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"
#include "core/network/NetworkServer.h"
#include "core/renderer/Renderer.h"
#include "core/scene/Scene.h"

// The using statement is fine because
// this is the main entrypoint
using namespace mondradiko;  // NOLINT

bool g_interrupted = false;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  g_interrupted = true;

  return;
}

int main(int argc, const char* argv[]) {
  TracyMessageL("Hello Tracy!");
  log_inf("Hello internet!");

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  try {
    Filesystem fs("../test-folder/");
    SdlDisplay display;
    GpuInstance gpu(&display);

    if (!display.createSession(&gpu)) {
      log_ftl("Failed to create display session!");
    }

    Renderer renderer(&display, &gpu);
    Scene scene(&display, &fs, &gpu, &renderer);
    NetworkServer server(&scene, "127.0.0.1", 10555);

    // scene.loadModel("DamagedHelmet.gltf");

    while (!g_interrupted) {
      if (!scene.update()) break;

      server.update();
    }

    display.destroySession();
  } catch (const std::exception& e) {
    log_err("Mondradiko server failed with message:");
    log_err(e.what());
    return 1;
  }

  return 0;
}
