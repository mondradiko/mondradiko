/**
 * @file NetworkServer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Hosts a port and communicates to connecting clients with scene events
 * and server metadata.
 * @date 2020-12-06
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <unordered_set>

#include "steam/isteamnetworkingsockets.h"

namespace mondradiko {

// Forward declarations
class Scene;

class NetworkServer {
 public:
  NetworkServer(Scene*, const char*, int);
  ~NetworkServer();

  void update();

 private:
  Scene* scene;

  ISteamNetworkingSockets* sockets;

  std::unordered_set<HSteamNetConnection> connections;

  void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t*);

  static void callback_ConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t*);

  HSteamListenSocket listen_socket;
};

}  // namespace mondradiko
