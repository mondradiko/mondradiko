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

enum class ClientState { Disconnected, Connecting, Authenticating, Connected };

class NetworkClient {
 public:
  NetworkClient(Scene*, const char*, int);
  ~NetworkClient();

  void update();
  void disconnect();

  ClientState state = ClientState::Disconnected;

 private:
  Scene* scene;

  ISteamNetworkingSockets* sockets;

  HSteamNetConnection connection = k_HSteamNetConnection_Invalid;

  void requestJoin();

  void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t*);

  static void callback_ConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t*);

  ClientId client_id;

  struct QueuedEvent {
    std::vector<uint8_t> data;
  };

  std::deque<QueuedEvent> event_queue;
};

}  // namespace mondradiko
