// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <deque>
#include <queue>

#include "core/cvars/CVarScope.h"
#include "core/network/NetworkShared.h"
#include "lib/include/flatbuffers_headers.h"
#include "lib/include/gamenetworkingsockets_include.h"
#include "types/containers/vector.h"

namespace mondradiko {

// Forward declarations
namespace protocol {
struct Announcement;
struct AssignClientId;
}  // namespace protocol

namespace core {

// Forward declarations
class Avatar;
class Filesystem;
class UserInterface;
class World;

enum class ClientState { Disconnected, Connecting, Joining, Joined };

class NetworkClient {
 public:
  static void initCVars(CVarScope*);

  NetworkClient(const CVarScope*, Filesystem*, UserInterface*, World*);
  ~NetworkClient();

  bool connect(const Avatar*, const char*, int);
  void update();
  void disconnect();

  ClientState state = ClientState::Disconnected;

 private:
  const CVarScope* cvars;
  Filesystem* fs;
  UserInterface* ui;
  World* world;

  ISteamNetworkingSockets* sockets;

  const Avatar* avatar;

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
  void updateAvatar();

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
    types::vector<uint8_t> data;
  };

  std::deque<QueuedEvent> event_queue;
};

}  // namespace core
}  // namespace mondradiko
