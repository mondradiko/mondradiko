// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <deque>
#include <queue>
#include <vector>

#include "core/network/NetworkShared.h"
#include "flatbuffers/flatbuffers.h"
#include "steam/isteamnetworkingsockets.h"

namespace mondradiko {

// Forward declarations
class Filesystem;
class World;

namespace protocol {
struct Announcement;
struct AssignClientId;
}  // namespace protocol

enum class ClientState { Disconnected, Connecting, Joining, Joined };

class NetworkClient {
 public:
  NetworkClient(Filesystem*, World*, const char*, int);
  ~NetworkClient();

  void update();
  void disconnect();

  ClientState state = ClientState::Disconnected;

 private:
  Filesystem* fs;
  World* world;

  ISteamNetworkingSockets* sockets;

  HSteamNetConnection connection = k_HSteamNetConnection_Invalid;

  //
  // Server event receive callbacks
  //
  void onAnnouncement(const protocol::Announcement*);
  void onAssignClientId(const protocol::AssignClientId*);

  //
  // Client event send methods
  //
  void requestJoin();

  //
  // Connection status change callbacks
  //
  void onConnecting(HSteamNetConnection);
  void onConnected(HSteamNetConnection);
  void onProblemDetected(HSteamNetConnection);
  void onClosedByPeer(HSteamNetConnection);
  void onDisconnect(HSteamNetConnection);

  //
  // Helper methods
  //
  void receiveEvents();
  void sendEvent(flatbuffers::FlatBufferBuilder&);
  void sendQueuedEvents();
  static void callback_ConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t*);

  ClientId client_id;

  struct QueuedEvent {
    std::vector<uint8_t> data;
  };

  std::deque<QueuedEvent> event_queue;
};

}  // namespace mondradiko
