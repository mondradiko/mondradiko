// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <deque>

#include "core/network/NetworkShared.h"
#include "core/world/World.h"
#include "lib/include/flatbuffers_headers.h"
#include "lib/include/gamenetworkingsockets_include.h"

namespace mondradiko {

// Forward declarations
namespace protocol {
struct AvatarUpdate;
struct JoinRequest;
}  // namespace protocol

namespace core {

// Forward declarations
class Avatar;
class Filesystem;
class WorldEventSorter;

class NetworkServer {
 public:
  NetworkServer(Filesystem*, WorldEventSorter*, const char*, int);
  ~NetworkServer();

  void sendTestEvent(EntityId);

  void updateWorld();
  void update();

 private:
  Filesystem* fs;
  WorldEventSorter* world_event_sorter;

  ISteamNetworkingSockets* sockets;

  struct ConnectedClient {
    ClientId id;
    HSteamNetConnection net_connection;
    bool is_joined;
    Avatar* avatar;
  };

  types::unordered_map<ClientId, ConnectedClient> connections;

  //
  // Client event receive methods
  //
  void onJoinRequest(ConnectedClient*, const protocol::JoinRequest*);
  void onAvatarUpdate(ConnectedClient*, const protocol::AvatarUpdate*);

  //
  // Server event send methods
  //
  void sendAnnouncement(types::string);
  void setClientId(ClientId);

  //
  // Connection status change callbacks
  //
  void onConnecting(types::string, HSteamNetConnection);
  void onConnected(types::string, HSteamNetConnection);
  void onProblemDetected(types::string, HSteamNetConnection);
  void onClosedByPeer(types::string, HSteamNetConnection);
  void onDisconnect(types::string, HSteamNetConnection);

  //
  // Helper methods
  //
  void receiveEvents();
  void sendWorldUpdates();
  void sendEvent(flatbuffers::FlatBufferBuilder&, ClientId);
  void sendQueuedEvents();
  static void callback_ConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t*);

  HSteamListenSocket listen_socket = k_HSteamListenSocket_Invalid;

  HSteamNetPollGroup poll_group = k_HSteamNetPollGroup_Invalid;

  struct QueuedEvent {
    ClientId destination;
    types::vector<uint8_t> data;
  };

  std::deque<QueuedEvent> event_queue;
};

}  // namespace core
}  // namespace mondradiko
