/**
 * @file NetworkClient.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Establishes communication to a server, receives
 * events from it, and sends input and queries back.
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

#include "network/NetworkClient.h"

#include <sstream>

#include "log/log.h"

namespace mondradiko {

NetworkClient::NetworkClient(const char* server_address, int port) {
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

NetworkClient::~NetworkClient() { disconnect(); }

void NetworkClient::update() {}

bool NetworkClient::readEvent(ClientEvent* event) {
  if (!event_queue.empty()) {
    *event = event_queue.front();
    event_queue.pop();
    return true;
  }

  return false;
}

void NetworkClient::disconnect() { state = ClientState::Disconnected; }

}  // namespace mondradiko
