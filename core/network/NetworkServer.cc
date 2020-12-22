/**
 * @file NetworkServer.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Hosts a port and communicates to connecting clients with scene events
 * and server metadata.
 * @date 2020-12-06
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/network/NetworkServer.h"

#include <string>

#include "core/scene/Scene.h"
#include "log/log.h"
#include "protocol/ClientEvent_generated.h"
#include "protocol/ServerEvent_generated.h"
#include "steam/isteamnetworkingutils.h"
#include "steam/steamnetworkingsockets.h"

namespace mondradiko {

// Nasty singleton
// GameNetworkingSockets doesn't give us a choice here
NetworkServer* g_server = nullptr;

NetworkServer::NetworkServer(Scene* scene, const char* server_address,
                             int server_port)
    : scene(scene) {
  log_zone;

  if (g_server) {
    log_ftl("NetworkServer instance already exists");
  }

  g_server = this;

  SteamDatagramErrMsg err;
  if (!GameNetworkingSockets_Init(nullptr, err)) {
    std::string message = err;
    log_ftl("Failed to initialize SteamNetworkingSockets with error: %s",
            message.c_str());
  }

  sockets = SteamNetworkingSockets();

  log_dbg("Listening on port %d", server_port);

  SteamNetworkingIPAddr address;
  address.Clear();
  address.m_port = server_port;

  SteamNetworkingConfigValue_t callback;
  callback.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                  reinterpret_cast<void*>(callback_ConnectionStatusChanged));

  listen_socket = sockets->CreateListenSocketIP(address, 1, &callback);

  if (listen_socket == k_HSteamListenSocket_Invalid) {
    log_ftl("Failed to listen at port %d", server_port);
  }

  poll_group = sockets->CreatePollGroup();

  if (poll_group == k_HSteamNetPollGroup_Invalid) {
    log_ftl("Failed to create poll group");
  }
}

NetworkServer::~NetworkServer() {
  log_zone;

  for (auto connection : connections) {
    sockets->CloseConnection(connection.second, 0, "Server shutdown", true);
  }

  if (poll_group != k_HSteamNetPollGroup_Invalid) {
    sockets->DestroyPollGroup(poll_group);
  }

  if (listen_socket != k_HSteamListenSocket_Invalid) {
    sockets->CloseListenSocket(listen_socket);
  }
}

void NetworkServer::update() {
  log_zone;

  sockets->RunCallbacks();

  while (true) {
    ISteamNetworkingMessage* incoming_msg = nullptr;
    int msg_num =
        sockets->ReceiveMessagesOnPollGroup(poll_group, &incoming_msg, 1);

    if (msg_num == 0) break;

    if (msg_num < 0) {
      log_err("Error receiving messages");
      break;
    }

    auto event = protocol::GetClientEvent(incoming_msg->GetData());

    log_dbg("Received client event");

    switch (event->type()) {
      case protocol::ClientEventType::NoMessage: {
        log_dbg("Received empty client event");
        break;
      }

      case protocol::ClientEventType::JoinRequest: {
        auto join_request = event->join_request();
        log_dbg("Client joined: %s", join_request->username()->c_str());
        break;
      }

      default: {
        log_err("Unhandled client event %d", event->type());
        break;
      }
    }  // switch (event->type())

    incoming_msg->Release();
  }

  if (event_queue.size() == 0) return;

  std::vector<QueuedEvent> baked_queue;
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

  std::vector<SteamNetworkingMessage_t*> outgoing_messages;
  outgoing_messages.reserve(baked_queue.size());

  for (uint32_t i = 0; i < baked_queue.size(); i++) {
    auto& event = baked_queue[i];
    auto destination = connections.find(event.destination);

    if (destination == connections.end()) {
      log_err("Invalid destination %d", destination->first);
      continue;
    }

    // TODO(marceline-cramer) Avoid copies
    size_t message_size = event.data.size();
    SteamNetworkingMessage_t* message =
        SteamNetworkingUtils()->AllocateMessage(message_size);
    memcpy(message->m_pData, event.data.data(), message_size);

    message->m_conn = destination->second;
    message->m_nFlags = k_nSteamNetworkingSend_Reliable;

    outgoing_messages.push_back(message);
  }

  sockets->SendMessages(outgoing_messages.size(), outgoing_messages.data(),
                        nullptr);
}

void NetworkServer::sendAnnouncement(std::string message) {
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

  uint8_t* event_data = builder.GetBufferPointer();
  size_t event_size = builder.GetSize();
  auto& event_buffer = event_queue.emplace_back();
  event_buffer.destination =
      static_cast<ClientId>(protocol::ClientId::AllClients);
  event_buffer.data.resize(event_size);
  memcpy(event_buffer.data.data(), event_data, event_size);
}

ClientId NetworkServer::createNewConnection(HSteamNetConnection connection) {
  ClientId new_id = static_cast<ClientId>(protocol::ClientId::FirstClient);

  while (true) {
    auto it = connections.find(new_id);
    if (it == connections.end()) break;
    new_id++;
  }

  connections.emplace(new_id, connection);

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

  uint8_t* event_data = builder.GetBufferPointer();
  size_t event_size = builder.GetSize();
  auto& event_buffer = event_queue.emplace_back();
  event_buffer.destination = new_id;
  event_buffer.data.resize(event_size);
  memcpy(event_buffer.data.data(), event_data, event_size);

  return new_id;
}

void NetworkServer::onConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  log_zone;

  std::string description = event->m_info.m_szConnectionDescription;

  switch (event->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting: {
      log_dbg("Connection request from %s", description.c_str());

      if (sockets->AcceptConnection(event->m_hConn) != k_EResultOK) {
        sockets->CloseConnection(event->m_hConn, 0, nullptr, false);
        log_err("Failed to accept connection from %s", description.c_str());
        break;
      }

      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      sockets->SetConnectionPollGroup(event->m_hConn, poll_group);
      ClientId new_id = createNewConnection(event->m_hConn);
      log_dbg("Client #%d connected", new_id);
      std::string connect_message = "Welcome client #" + std::to_string(new_id);
      sendAnnouncement(connect_message);
      break;
    }

    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    case k_ESteamNetworkingConnectionState_ClosedByPeer: {
      if (event->m_eOldState ==
          k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
        log_dbg("Connection closed: Problem detected locally");
      } else {
        log_dbg("Connection closed: Peer closed connection");
      }

      ClientId client_id = 0;

      for (auto& it : connections) {
        if (it.second == event->m_hConn) {
          client_id = it.first;
        }
      }

      if (client_id != 0) {
        log_dbg("Client #%d disconnected", client_id);
        connections.erase(client_id);
      }

      sockets->CloseConnection(event->m_hConn, 0, nullptr, false);

      break;
    }

    case k_ESteamNetworkingConnectionState_None: {
      // Called when connections are destroyed
      break;
    }

    default: {
      log_wrn("Uncaught connection status change %s", description.c_str());
      break;
    }
  }
}

void NetworkServer::callback_ConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  g_server->onConnectionStatusChanged(event);
}

}  // namespace mondradiko
