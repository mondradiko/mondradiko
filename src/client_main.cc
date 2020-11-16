/**
 * @file client_main.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Entrypoint for a command-line client session.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include <csignal>
#include <iostream>
#include <sstream>

#include "displays/OpenXrDisplay.h"
#include "displays/SdlDisplay.h"
#include "filesystem/Filesystem.h"
#include "graphics/Renderer.h"
#include "graphics/VulkanInstance.h"
#include "log/log.h"
#include "network/NetworkClient.h"
#include "scene/Scene.h"
#include "src/api_headers.h"

using namespace mondradiko;

bool g_interrupted = false;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  g_interrupted = true;

  return;
}

void player_session_run(const char* server_address, int port) {
  Filesystem fs("../test-folder/");
  OpenXrDisplay display;
  VulkanInstance vulkan_instance(&display);

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  if (!display.createSession(&vulkan_instance)) {
    log_ftl("Failed to create display session!");
  }

  Renderer renderer(&display, &vulkan_instance);
  NetworkClient client(server_address, port);
  Scene scene(&fs, &renderer);

  // Break is triggered on Ctrl+C or SIGTERM
  while (!g_interrupted) {
    DisplayPollEventsInfo poll_info;
    poll_info.renderer = &renderer;

    display.pollEvents(&poll_info);
    if (poll_info.should_quit) break;

    if (poll_info.should_run) {
      DisplayBeginFrameInfo frame_info;
      display.beginFrame(&frame_info);

      client.update();
      scene.update(frame_info.dt);

      ClientEvent event;
      while (client.readEvent(&event)) {
        log_dbg("Received client event.");
      }

      if (frame_info.should_render) {
        renderer.renderFrame();
      }

      display.endFrame(&frame_info);
    }
  }

  display.destroySession();
}

void spectator_session_run(const char* server_address, int port) {
  Filesystem fs("../test-folder/");
  SdlDisplay display;
  VulkanInstance vulkan_instance(&display);

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  if (!display.createSession(&vulkan_instance)) {
    log_ftl("Failed to create display session!");
  }

  Renderer renderer(&display, &vulkan_instance);
  NetworkClient client(server_address, port);
  Scene scene(&fs, &renderer);

  scene.loadModel("DamagedHelmet.gltf");

  // Break is triggered on Ctrl+C or SIGTERM
  while (!g_interrupted) {
    DisplayPollEventsInfo poll_info;
    poll_info.renderer = &renderer;

    display.pollEvents(&poll_info);
    if (poll_info.should_quit) break;

    if (poll_info.should_run) {
      DisplayBeginFrameInfo frame_info;
      display.beginFrame(&frame_info);

      client.update();
      scene.update(frame_info.dt);

      ClientEvent event;
      while (client.readEvent(&event)) {
        log_dbg("Received client event.");
      }

      if (frame_info.should_render) {
        renderer.renderFrame();
      }

      display.endFrame(&frame_info);
    }
  }

  display.destroySession();
}

int main(int argc, const char* argv[]) {
  log_inf("Hello VR!");

  PHYSFS_init(argv[0]);

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
