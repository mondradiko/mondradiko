/**
 * @file NetworkClient.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Establishes communication to a server, receives
 * events from it, and sends input and queries back.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/network/NetworkClient.h"

#include <sstream>

#include "core/scene/Scene.h"
#include "log/log.h"

namespace mondradiko {

NetworkClient::NetworkClient(Scene* scene, const char* server_address, int port)
    : scene(scene) {
  log_zone;

  state = ClientState::Connecting;

  if (!connect(server_address, port)) {
    disconnect();
    std::ostringstream err;
    err << "Failed to connect to server ";
    err << server_address << ":" << port << ".";
    log_ftl(err.str().c_str());
  }

  state = ClientState::Authenticating;

  if (!authenticate()) {
    disconnect();
    std::ostringstream err;
    err << "Failed to authenticate to server ";
    err << server_address << ":" << port << ".";
    log_ftl(err.str().c_str());
  }

  state = ClientState::Connected;
}

NetworkClient::~NetworkClient() {
  log_zone;

  disconnect();
}

void NetworkClient::update() {}

bool NetworkClient::readEvent(ClientEvent* event) {
  if (!event_queue.empty()) {
    *event = event_queue.front();
    event_queue.pop();
    return true;
  }

  return false;
}

void NetworkClient::disconnect() {
  log_zone;

  state = ClientState::Disconnected;
}

}  // namespace mondradiko
