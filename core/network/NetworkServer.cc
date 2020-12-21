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
}

NetworkServer::~NetworkServer() {
  log_zone;

  for (auto connection : connections) {
    sockets->CloseConnection(connection, 0, "Server shutdown", true);
  }

  if (listen_socket != k_HSteamListenSocket_Invalid) {
    sockets->CloseListenSocket(listen_socket);
  }
}

void NetworkServer::update() {
  log_zone;

  sockets->RunCallbacks();

  if (event_queue.size() == 0) return;

  for (auto client : connections) {
    std::vector<SteamNetworkingMessage_t*> outgoing_messages(
        event_queue.size());

    for (uint32_t i = 0; i < outgoing_messages.size(); i++) {
      auto& message = outgoing_messages[i];
      auto& event = event_queue[i];

      // TODO(marceline-cramer) Avoid copies
      size_t message_size = event.size();
      message = SteamNetworkingUtils()->AllocateMessage(message_size);
      memcpy(message->m_pData, event.data(), message_size);

      message->m_conn = client;
      message->m_nFlags = k_nSteamNetworkingSend_Reliable;
    }

    sockets->SendMessages(outgoing_messages.size(), outgoing_messages.data(),
                          nullptr);
  }

  event_queue.clear();
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

  log_inf("Buffer size %d", event_size);

  auto& event_buffer = event_queue.emplace_back(event_size);
  memcpy(event_buffer.data(), event_data, event_size);
}

void NetworkServer::onConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  log_zone;

  std::string description = event->m_info.m_szConnectionDescription;

  switch (event->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting: {
      log_dbg("Connection request");

      if (sockets->AcceptConnection(event->m_hConn) != k_EResultOK) {
        sockets->CloseConnection(event->m_hConn, 0, nullptr, false);
        log_err("Failed to accept connection from %s", description.c_str());
        break;
      }

      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      log_dbg("Connected");
      connections.emplace(event->m_hConn);
      sendAnnouncement("Hello client!");
      sendAnnouncement("Welcome to Mondradiko :)");
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

      connections.erase(event->m_hConn);
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
