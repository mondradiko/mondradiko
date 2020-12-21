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

#pragma once

#include <queue>

#include "core/network/NetworkShared.h"
#include "steam/isteamnetworkingsockets.h"

namespace mondradiko {

// Forward declarations
class Scene;

enum class ClientEventType { Ping };

struct ClientEvent {
  ClientEventType type;
};

enum class ClientState { Disconnected, Connecting, Authenticating, Connected };

class NetworkClient {
 public:
  NetworkClient(Scene*, const char*, int);
  ~NetworkClient();

  void update();
  bool readEvent(ClientEvent*);
  void disconnect();

  ClientState state = ClientState::Disconnected;

 private:
  Scene* scene;

  ISteamNetworkingSockets* sockets;

  HSteamNetConnection connection = k_HSteamNetConnection_Invalid;

  bool connect(const char*, int);
  bool authenticate();

  void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t*);

  static void callback_ConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t*);

  std::queue<ClientEvent> event_queue;

  ClientId client_id;
};

}  // namespace mondradiko
