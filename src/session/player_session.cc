/**
 * @file player_session.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Runs the lifetime of a single Mondradiko player session.
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

#include "session/player_session.h"

#include <csignal>
#include <iostream>
#include <sstream>

#include "filesystem/Filesystem.h"
#include "graphics/Renderer.h"
#include "graphics/VulkanInstance.h"
#include "log/log.h"
#include "network/NetworkClient.h"
#include "scene/Scene.h"
#include "xr/PlayerSession.h"
#include "xr/XrDisplay.h"

bool shouldQuit = false;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
  shouldQuit = true;

  return;
}

void player_session_run(const char *server_address, int port) {
  mondradiko::Filesystem fs("../test-folder/");
  mondradiko::XrDisplay xr;
  mondradiko::VulkanInstance vulkan_instance(&xr);
  mondradiko::PlayerSession session(&xr, &vulkan_instance);
  mondradiko::Renderer renderer(&vulkan_instance, &session);
  mondradiko::NetworkClient client(server_address, port);
  mondradiko::Scene scene(&fs, &renderer);

  scene.loadModel("DamagedHelmet.gltf");

  if (signal(SIGTERM, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGTERM");
  }

  if (signal(SIGINT, signalHandler) == SIG_ERR) {
    log_wrn("Can't catch SIGINT");
  }

  bool shouldRun = false;

  while (true) {
    session.pollEvents(&shouldRun, &shouldQuit);
    if (shouldQuit)
      break;

    if (shouldRun) {
      double dt;
      bool shouldRender;
      session.beginFrame(&dt, &shouldRender);

      client.update();
      scene.update(dt);

      mondradiko::ClientEvent event;
      while (client.readEvent(&event)) {
        log_dbg("Received client event.");
      }

      if (shouldRender) {
        renderer.renderFrame();
      }

      session.endFrame(&renderer, shouldRender);
    }
  }
}
