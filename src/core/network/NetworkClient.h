/**
 * @file NetworkClient.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Establishes communication to a server, receives
 * events from it, and sends input and queries back.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_NETWORK_NETWORKCLIENT_H_
#define SRC_NETWORK_NETWORKCLIENT_H_

#include <queue>

#include "network/NetworkShared.h"

namespace mondradiko {

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
  std::queue<ClientEvent> event_queue;
};

}  // namespace mondradiko

#endif  // SRC_NETWORK_NETWORKCLIENT_H_
