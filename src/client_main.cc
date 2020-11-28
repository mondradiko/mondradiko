/**
 * @file client_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Entrypoint for a command-line client session.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include <csignal>
#include <iostream>
#include <sstream>

#include "displays/OpenXrDisplay.h"
#include "displays/SdlDisplay.h"
#include "filesystem/Filesystem.h"
#include "gpu/GpuInstance.h"
#include "log/log.h"
#include "network/NetworkClient.h"
#include "renderer/Renderer.h"
#include "scene/Scene.h"
#include "src/api_headers.h"

// The using statement is fine because
// this is the main entrypoint
using namespace mondradiko;  // NOLINT

bool g_interrupted = false;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  g_interrupted = true;

  return;
}

void session_loop(Filesystem* fs, DisplayInterface* display, GpuInstance* gpu,
                  NetworkClient* client) {
  if (!display->createSession(gpu)) {
    log_ftl("Failed to create display session!");
  }

  Renderer renderer(display, gpu);
  Scene scene(fs, &renderer);

  scene.loadModel("DamagedHelmet.gltf");

  // Break is triggered on Ctrl+C or SIGTERM
  g_interrupted = false;
  while (!g_interrupted) {
    DisplayPollEventsInfo poll_info;
    poll_info.renderer = &renderer;

    display->pollEvents(&poll_info);
    if (poll_info.should_quit) break;

    if (poll_info.should_run) {
      DisplayBeginFrameInfo frame_info;
      display->beginFrame(&frame_info);

      client->update();
      scene.update(frame_info.dt);

      ClientEvent event;
      while (client->readEvent(&event)) {
        log_dbg("Received client event.");
      }

      if (frame_info.should_render) {
        renderer.renderFrame();
      }

      display->endFrame(&frame_info);
    }
  }

  display->destroySession();
}

void player_session_run(const char* server_address, int port) {
  Filesystem fs("../test-folder/");
  OpenXrDisplay display;
  GpuInstance gpu(&display);
  NetworkClient client(server_address, port);

  session_loop(&fs, &display, &gpu, &client);
}

void spectator_session_run(const char* server_address, int port) {
  Filesystem fs("../test-folder/");
  SdlDisplay display;
  GpuInstance gpu(&display);
  NetworkClient client(server_address, port);

  session_loop(&fs, &display, &gpu, &client);
}

int main(int argc, const char* argv[]) {
  TracyMessageL("Hello Tracy!");
  log_inf("Hello VR!");

  PHYSFS_init(argv[0]);

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
    PHYSFS_deinit();
    return 1;
  }

  PHYSFS_deinit();
  return 0;
}
