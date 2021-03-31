// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/network/NetworkServer.h"

#include <memory>

#include "core/avatars/Avatar.h"
#include "core/avatars/SpectatorAvatar.h"
#include "core/filesystem/Filesystem.h"
#include "core/world/World.h"
#include "core/world/WorldEventSorter.h"
#include "log/log.h"
#include "types/protocol/ClientEvent_generated.h"
#include "types/protocol/ServerEvent_generated.h"
#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

// Nasty singleton
// GameNetworkingSockets doesn't give us a choice here
NetworkServer* g_server = nullptr;

NetworkServer::NetworkServer(Filesystem* fs,
                             WorldEventSorter* world_event_sorter,
                             const char* server_address, int server_port)
    : fs(fs), world_event_sorter(world_event_sorter) {
  log_zone;

  if (g_server) {
    log_ftl("NetworkServer instance already exists");
  }

  g_server = this;

  SteamDatagramErrMsg err;
  if (!GameNetworkingSockets_Init(nullptr, err)) {
    types::string message = err;
    log_ftl_fmt("Failed to initialize SteamNetworkingSockets with error: %s",
                message.c_str());
  }

  sockets = SteamNetworkingSockets();

  SteamNetworkingIPAddr address;
  address.Clear();
  if (!address.ParseString(server_address)) {
    log_ftl_fmt("Failed to parse server address %s", server_address);
  }
  address.m_port = server_port;

  char szAddr[SteamNetworkingIPAddr::k_cchMaxString];
  address.ToString(szAddr, sizeof(szAddr), true);
  types::string addr_string = szAddr;
  log_msg_fmt("Listening on %s", addr_string.c_str());

  SteamNetworkingConfigValue_t callback;
  callback.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                  reinterpret_cast<void*>(callback_ConnectionStatusChanged));

  listen_socket = sockets->CreateListenSocketIP(address, 1, &callback);

  if (listen_socket == k_HSteamListenSocket_Invalid) {
    log_ftl_fmt("Failed to listen on %s", addr_string.c_str());
  }

  poll_group = sockets->CreatePollGroup();

  if (poll_group == k_HSteamNetPollGroup_Invalid) {
    log_ftl("Failed to create poll group");
  }
}

NetworkServer::~NetworkServer() {
  log_zone;

  for (auto iter : connections) {
    auto& conn = iter.second;
    sockets->CloseConnection(conn.net_connection, 0, "Server shutdown", true);
    if (conn.avatar != nullptr) delete conn.avatar;
  }

  if (poll_group != k_HSteamNetPollGroup_Invalid) {
    sockets->DestroyPollGroup(poll_group);
  }

  if (listen_socket != k_HSteamListenSocket_Invalid) {
    sockets->CloseListenSocket(listen_socket);
  }
}

void NetworkServer::updateWorld() {
  sendWorldUpdates();
  world_event_sorter->clearQueue();
}

void NetworkServer::update() {
  log_zone;

  sockets->RunCallbacks();

  receiveEvents();

  if (world_event_sorter->isOutOfDate()) {
    sendWorldUpdates();
    world_event_sorter->clearQueue();
  }

  if (event_queue.size() > 0) {
    sendQueuedEvents();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Client event receive callbacks
///////////////////////////////////////////////////////////////////////////////

void NetworkServer::onJoinRequest(ConnectedClient* client,
                                  const protocol::JoinRequest* join_request) {
  if (client->is_joined) {
    log_err_fmt("Client #%du has already joined", client->id);
    return;
  }

  types::vector<assets::LumpHash> our_checksums;
  fs->getChecksums(our_checksums);

  log_dbg("Checking client lump checksums");

  bool check_passed = true;

  if (join_request->lump_checksums()->size() != our_checksums.size()) {
    log_dbg_fmt("Checksum count mismatch (we have %lu, they have %du)",
                our_checksums.size(), join_request->lump_checksums()->size());
    check_passed = false;
  } else {
    for (uint32_t i = 0; i < our_checksums.size(); i++) {
      log_dbg_fmt("Checking checksum %d: 0x%016lx", i, our_checksums[i]);
      auto their_checksum = join_request->lump_checksums()->Get(i);
      if (our_checksums[i] != static_cast<assets::LumpHash>(their_checksum)) {
        log_dbg_fmt("Checksum mismatch (client has 0x%016lx)", their_checksum);
        check_passed = false;
        break;
      }
    }
  }

  Avatar* client_avatar = nullptr;
  switch (join_request->avatar_type()) {
    case protocol::AvatarType::Spectator: {
      log_inf_fmt("Client is using SpectatorAvatar");
      client_avatar = new SpectatorAvatar(world_event_sorter->getWorld());
      break;
    }

    default: {
      log_err_fmt("Unrecognized AvatarType: %hu", join_request->avatar_type());
      break;
    }
  }

  if (client_avatar != nullptr && check_passed) {
    client->is_joined = true;
    client->avatar = client_avatar;

    log_msg_fmt("Client joined: %s", join_request->username()->c_str());
    types::string connect_message =
        "Welcome client #" + std::to_string(client->id);
    sendAnnouncement(connect_message);
  } else {
    log_msg_fmt("Client join request denied: %du", client->id);
  }
}

void NetworkServer::onAvatarUpdate(
    ConnectedClient* client, const protocol::AvatarUpdate* avatar_update) {
  if (client->avatar == nullptr) {
    log_err_fmt("Avatar %du doesn't have an avatar", client->id);
    return;
  }

  client->avatar->deserialize(avatar_update->data());
}

///////////////////////////////////////////////////////////////////////////////
// Server event send methods
///////////////////////////////////////////////////////////////////////////////

void NetworkServer::sendAnnouncement(types::string message) {
  flatbuffers::FlatBufferBuilder builder;
  builder.Clear();

  auto message_offset = builder.CreateString(message);

  protocol::AnnouncementBuilder announcement(builder);
  announcement.add_message(message_offset);
  auto announcement_offset = announcement.Finish();

  protocol::ServerEventBuilder event(builder);
  event.add_type(protocol::ServerEventType::Announcement);
  event.add_announcement(announcement_offset);
  auto event_offset = event.Finish();

  builder.Finish(event_offset);
  sendEvent(builder, static_cast<ClientId>(protocol::ClientId::AllClients));
}

void NetworkServer::setClientId(ClientId new_id) {
  flatbuffers::FlatBufferBuilder builder;
  builder.Clear();

  protocol::AssignClientIdBuilder assign_client_id(builder);
  assign_client_id.add_new_id(
      static_cast<mondradiko::protocol::ClientId>(new_id));
  auto assign_client_id_offset = assign_client_id.Finish();

  protocol::ServerEventBuilder event(builder);
  event.add_type(protocol::ServerEventType::AssignClientId);
  event.add_assign_client_id(assign_client_id_offset);
  auto event_offset = event.Finish();

  builder.Finish(event_offset);
  sendEvent(builder, new_id);
}

///////////////////////////////////////////////////////////////////////////////
// Connection status change callbacks
///////////////////////////////////////////////////////////////////////////////

void NetworkServer::onConnecting(types::string description,
                                 HSteamNetConnection connection) {
  log_msg_fmt("Connection request from %s", description.c_str());

  if (sockets->AcceptConnection(connection) != k_EResultOK) {
    sockets->CloseConnection(connection, 0, nullptr, false);
    log_err_fmt("Failed to accept connection from %s", description.c_str());
  }
}

void NetworkServer::onConnected(types::string description,
                                HSteamNetConnection connection) {
  sockets->SetConnectionPollGroup(connection, poll_group);

  ClientId new_id = static_cast<ClientId>(protocol::ClientId::FirstClient);

  while (true) {
    auto it = connections.find(new_id);
    if (it == connections.end()) break;
    new_id++;
  }

  ConnectedClient new_client{};
  new_client.id = new_id;
  new_client.net_connection = connection;
  new_client.is_joined = false;
  new_client.avatar = nullptr;
  connections.emplace(new_id, new_client);

  log_msg_fmt("Client #%d connected", new_id);
}

void NetworkServer::onProblemDetected(types::string description,
                                      HSteamNetConnection connection) {
  log_err("Connection closed: Problem detected locally");
}

void NetworkServer::onClosedByPeer(types::string description,
                                   HSteamNetConnection connection) {
  log_err("Connection closed: Peer closed connection");
}

void NetworkServer::onDisconnect(types::string description,
                                 HSteamNetConnection connection) {
  ClientId client_id = 0;

  for (auto& iter : connections) {
    if (iter.second.net_connection == connection) {
      client_id = iter.first;
      break;
    }
  }

  if (client_id != 0) {
    log_msg_fmt("Client #%d disconnected", client_id);

    auto& client = connections.at(client_id);
    if (client.avatar != nullptr) delete client.avatar;

    connections.erase(client_id);
  }

  sockets->CloseConnection(connection, 0, nullptr, false);
}

///////////////////////////////////////////////////////////////////////////////
// Helper methods
///////////////////////////////////////////////////////////////////////////////

void NetworkServer::receiveEvents() {
  while (true) {
    ISteamNetworkingMessage* incoming_msg = nullptr;
    int msg_num =
        sockets->ReceiveMessagesOnPollGroup(poll_group, &incoming_msg, 1);

    if (msg_num == 0) break;

    if (msg_num < 0) {
      log_err("Error receiving messages");
      break;
    }

    ConnectedClient* client = nullptr;
    for (auto& iter : connections) {
      if (iter.second.net_connection == incoming_msg->m_conn) {
        client = &iter.second;
        break;
      }
    }

    if (client == nullptr) continue;

    auto event = protocol::GetClientEvent(incoming_msg->GetData());

    switch (event->type()) {
      case protocol::ClientEventType::NoMessage: {
        log_dbg("Received empty client event");
        break;
      }

      case protocol::ClientEventType::JoinRequest: {
        onJoinRequest(client, event->join_request());
        break;
      }

      case protocol::ClientEventType::AvatarUpdate: {
        onAvatarUpdate(client, event->avatar_update());
        break;
      }

      default: {
        log_err_fmt("Unhandled client event %hu", event->type());
        break;
      }
    }  // switch (event->type())

    incoming_msg->Release();
  }
}

void NetworkServer::sendWorldUpdates() {
  flatbuffers::FlatBufferBuilder builder;

  auto update_offset = world_event_sorter->broadcastGlobalEvents(builder);

  protocol::ServerEventBuilder event_builder(builder);
  event_builder.add_type(protocol::ServerEventType::WorldUpdate);
  event_builder.add_world_update(update_offset);
  auto event_offset = event_builder.Finish();

  builder.Finish(event_offset);
  sendEvent(builder, static_cast<ClientId>(protocol::ClientId::AllClients));
}

void NetworkServer::sendEvent(flatbuffers::FlatBufferBuilder& builder,
                              ClientId destination) {
  uint8_t* event_data = builder.GetBufferPointer();
  size_t event_size = builder.GetSize();
  auto& event_buffer = event_queue.emplace_back();
  event_buffer.destination = destination;
  event_buffer.data.resize(event_size);
  memcpy(event_buffer.data.data(), event_data, event_size);
}

void NetworkServer::sendQueuedEvents() {
  types::vector<QueuedEvent> baked_queue;
  baked_queue.reserve(event_queue.size());

  // TODO(marceline-cramer) Avoid duplicating event buffers

  for (const auto& event : event_queue) {
    if (event.destination ==
        static_cast<ClientId>(protocol::ClientId::AllClients)) {
      for (auto& client : connections) {
        QueuedEvent subevent(event);
        subevent.destination = client.first;
        baked_queue.push_back(subevent);
      }
    } else {
      baked_queue.push_back(event);
    }
  }

  event_queue.clear();

  types::vector<SteamNetworkingMessage_t*> outgoing_messages;
  outgoing_messages.reserve(baked_queue.size());

  for (uint32_t i = 0; i < baked_queue.size(); i++) {
    auto& event = baked_queue[i];
    auto destination = connections.find(event.destination);

    if (destination == connections.end()) {
      log_err_fmt("Invalid destination %d", destination->first);
      continue;
    }

    // TODO(marceline-cramer) Avoid copies
    size_t message_size = event.data.size();
    SteamNetworkingMessage_t* message =
        SteamNetworkingUtils()->AllocateMessage(message_size);
    memcpy(message->m_pData, event.data.data(), message_size);

    message->m_conn = destination->second.net_connection;
    message->m_nFlags = k_nSteamNetworkingSend_Reliable;

    outgoing_messages.push_back(message);
  }

  sockets->SendMessages(outgoing_messages.size(), outgoing_messages.data(),
                        nullptr);
}

void NetworkServer::callback_ConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  log_zone;

  types::string description = event->m_info.m_szConnectionDescription;

  switch (event->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting: {
      g_server->onConnecting(description, event->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      g_server->onConnected(description, event->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    case k_ESteamNetworkingConnectionState_ClosedByPeer: {
      if (event->m_eOldState ==
          k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
        g_server->onProblemDetected(description, event->m_hConn);
      } else {
        g_server->onClosedByPeer(description, event->m_hConn);
      }

      g_server->onDisconnect(description, event->m_hConn);

      break;
    }

    case k_ESteamNetworkingConnectionState_None: {
      // Called when connections are destroyed
      break;
    }

    default: {
      log_err_fmt("Uncaught connection status change %s", description.c_str());
      break;
    }
  }
}

}  // namespace core
}  // namespace mondradiko
