/**
 * @file NetworkClient.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Establishes communication to a server, receives
 * events from it, and sends input and queries back.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/network/NetworkClient.h"

#include <sstream>
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
NetworkClient* g_client = nullptr;

NetworkClient::NetworkClient(Scene* scene, const char* server_ip,
                             int server_port)
    : scene(scene) {
  log_zone;

  if (g_client) {
    log_ftl("NetworkClient already exists");
  }

  g_client = this;

  SteamDatagramErrMsg err;
  if (!GameNetworkingSockets_Init(nullptr, err)) {
    std::string message = err;
    log_ftl("Failed to initialize SteamNetworkingSockets with error: %s",
            message.c_str());
  }

  sockets = SteamNetworkingSockets();

  SteamNetworkingIPAddr server_addr;
  server_addr.Clear();

  if (!server_addr.ParseString(server_ip)) {
    log_ftl("Failed to parse server address %s", server_ip);
  }

  server_addr.m_port = server_port;

  char szAddr[SteamNetworkingIPAddr::k_cchMaxString];
  server_addr.ToString(szAddr, sizeof(szAddr), true);
  std::string addr_string = szAddr;
  log_dbg("Connecting to server at %s", addr_string.c_str());

  SteamNetworkingConfigValue_t callback;
  callback.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                  reinterpret_cast<void*>(callback_ConnectionStatusChanged));
  connection = sockets->ConnectByIPAddress(server_addr, 1, &callback);

  if (connection == k_HSteamNetConnection_Invalid) {
    log_ftl("Failed to create connection");
  }
}

NetworkClient::~NetworkClient() {
  log_zone;

  disconnect();
}

void NetworkClient::update() {
  log_zone;

  sockets->RunCallbacks();

  if (connection == k_HSteamNetConnection_Invalid) return;

  receiveEvents();

  if (event_queue.size() > 0) {
    sendQueuedEvents();
  }
}

void NetworkClient::disconnect() {
  log_zone;

  if (connection != k_HSteamNetConnection_Invalid) {
    sockets->CloseConnection(connection, 0, "Client disconnect", true);
    connection = k_HSteamNetConnection_Invalid;
  }

  state = ClientState::Disconnected;
}

///////////////////////////////////////////////////////////////////////////////
// Server event receive callbacks
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::onAnnouncement(const protocol::Announcement* announcement) {
  log_dbg("Server announcement: %s", announcement->message()->c_str());
}

void NetworkClient::onAssignClientId(
    const protocol::AssignClientId* assign_client_id) {
  client_id = static_cast<ClientId>(assign_client_id->new_id());
  log_dbg("Assigned new ID #%d", client_id);
  state = ClientState::Joined;
}

///////////////////////////////////////////////////////////////////////////////
// Client event send methods
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::requestJoin() {
  flatbuffers::FlatBufferBuilder builder;
  builder.Clear();

  auto username_offset = builder.CreateString("TestClient");

  protocol::JoinRequestBuilder join_request(builder);
  join_request.add_username(username_offset);
  auto join_request_offset = join_request.Finish();

  protocol::ClientEventBuilder event(builder);
  event.add_type(protocol::ClientEventType::JoinRequest);
  event.add_join_request(join_request_offset);
  auto event_offset = event.Finish();

  builder.Finish(event_offset);
  sendEvent(builder);
}

///////////////////////////////////////////////////////////////////////////////
// Connection status change callbacks
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::onConnecting(HSteamNetConnection) {
  log_dbg("Connecting");
  state = ClientState::Connecting;
}

void NetworkClient::onConnected(HSteamNetConnection) {
  log_dbg("Connected");
  requestJoin();
}

void NetworkClient::onProblemDetected(HSteamNetConnection) {
  log_err("Failed to reach remote host");
}

void NetworkClient::onClosedByPeer(HSteamNetConnection) {
  log_err("Problem detected locally");
}

void NetworkClient::onDisconnect(HSteamNetConnection connection) {
  log_err("Forcefully disconnected");
  sockets->CloseConnection(connection, 0, nullptr, false);
  connection = k_HSteamNetConnection_Invalid;
  state = ClientState::Disconnected;
}

///////////////////////////////////////////////////////////////////////////////
// Helper methods
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::receiveEvents() {
  while (true) {
    ISteamNetworkingMessage* incoming_msg = nullptr;
    int msg_num =
        sockets->ReceiveMessagesOnConnection(connection, &incoming_msg, 1);

    if (msg_num == 0) break;

    if (msg_num < 0) {
      log_err("Error receiving messages");
      break;
    }

    auto event = protocol::GetServerEvent(incoming_msg->GetData());

    log_dbg("Received server event");

    switch (event->type()) {
      case protocol::ServerEventType::NoMessage: {
        log_dbg("Received empty server event");
        break;
      }

      case protocol::ServerEventType::Announcement: {
        onAnnouncement(event->announcement());
        break;
      }

      case protocol::ServerEventType::AssignClientId: {
        onAssignClientId(event->assign_client_id());
        break;
      }

      default: {
        log_err("Unhandled server event %d", event->type());
        break;
      }
    }  // switch (event->type())

    incoming_msg->Release();
  }
}

void NetworkClient::sendEvent(flatbuffers::FlatBufferBuilder& builder) {
  uint8_t* event_data = builder.GetBufferPointer();
  size_t event_size = builder.GetSize();
  auto& event_buffer = event_queue.emplace_back();
  event_buffer.data.resize(event_size);
  memcpy(event_buffer.data.data(), event_data, event_size);
}

void NetworkClient::sendQueuedEvents() {
  std::vector<SteamNetworkingMessage_t*> outgoing_messages;
  outgoing_messages.reserve(event_queue.size());

  for (uint32_t i = 0; i < event_queue.size(); i++) {
    auto& event = event_queue[i];

    // TODO(marceline-cramer) Avoid copies
    size_t message_size = event.data.size();
    SteamNetworkingMessage_t* message =
        SteamNetworkingUtils()->AllocateMessage(message_size);
    memcpy(message->m_pData, event.data.data(), message_size);

    message->m_conn = connection;
    message->m_nFlags = k_nSteamNetworkingSend_Reliable;

    outgoing_messages.push_back(message);
  }

  sockets->SendMessages(outgoing_messages.size(), outgoing_messages.data(),
                        nullptr);

  event_queue.clear();
}

void NetworkClient::callback_ConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  log_zone;

  std::string description = event->m_info.m_szConnectionDescription;

  switch (event->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting: {
      g_client->onConnecting(event->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      g_client->onConnected(event->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    case k_ESteamNetworkingConnectionState_ClosedByPeer: {
      if (event->m_eOldState ==
          k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
        g_client->onProblemDetected(event->m_hConn);
      } else if (event->m_eOldState ==
                 k_ESteamNetworkingConnectionState_Connecting) {
        g_client->onClosedByPeer(event->m_hConn);
      }

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

}  // namespace mondradiko
