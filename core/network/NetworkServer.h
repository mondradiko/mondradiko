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

#include <deque>
#include <string>
#include <unordered_map>

#include "core/network/NetworkShared.h"
#include "flatbuffers/flatbuffers.h"
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

  std::unordered_map<ClientId, HSteamNetConnection> connections;

  void sendAnnouncement(std::string);
  ClientId createNewConnection(HSteamNetConnection);

  void onConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t*);

  static void callback_ConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t*);

  HSteamListenSocket listen_socket;

  struct QueuedEvent {
    ClientId destination;
    std::vector<uint8_t> data;
  };

  std::deque<QueuedEvent> event_queue;
};

}  // namespace mondradiko
