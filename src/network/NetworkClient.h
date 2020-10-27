/**
 * @file NetworkClient.h
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

#ifndef SRC_NETWORK_NETWORKCLIENT_H_
#define SRC_NETWORK_NETWORKCLIENT_H_

#include <queue>

#include "network/NetworkShared.h"

enum class ClientEventType { Ping };

struct ClientEvent {
  ClientEventType type;
};

enum class ClientState { Disconnected, Connecting, Authenticating, Connected };

class NetworkClient : public NetworkShared {
 public:
  NetworkClient(const char*, int);
  ~NetworkClient();

  void update();
  bool readEvent(ClientEvent*);
  void disconnect();

  ClientState state = ClientState::Disconnected;

 private:
  std::queue<ClientEvent> eventQueue;
};

#endif  // SRC_NETWORK_NETWORKCLIENT_H_
