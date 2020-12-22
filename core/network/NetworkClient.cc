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
        auto announcement = event->announcement();

        log_dbg("Server announcement: %s", announcement->message()->c_str());
        break;
      }

      case protocol::ServerEventType::AssignClientId: {
        auto assign_client_id = event->assign_client_id();
        client_id = static_cast<ClientId>(assign_client_id->new_id());
        log_dbg("Assigned new ID #%d", client_id);
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

bool NetworkClient::readEvent(ClientEvent* event) {
  if (!event_queue.empty()) {
    *event = event_queue.front();
    event_queue.pop();
    return true;
  }

  return false;
}

void NetworkClient::disconnect() {
  log_zone;

  if (connection != k_HSteamNetConnection_Invalid) {
    sockets->CloseConnection(connection, 0, "Client disconnect", true);
    connection = k_HSteamNetConnection_Invalid;
  }

  state = ClientState::Disconnected;
}

void NetworkClient::onConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  log_zone;

  std::string description = event->m_info.m_szConnectionDescription;

  switch (event->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting: {
      log_dbg("Connecting");
      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      log_dbg("Connected");
      break;
    }

    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    case k_ESteamNetworkingConnectionState_ClosedByPeer: {
      log_err("Connection closed: %s", description.c_str());

      if (event->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
        log_err("Failed to reach remote host");
      } else if (event->m_eOldState ==
                 k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
        log_err("Problem detected locally");
      } else {
        log_err("Connection closed by server");
      }

      sockets->CloseConnection(event->m_hConn, 0, nullptr, false);
      connection = k_HSteamNetConnection_Invalid;

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

void NetworkClient::callback_ConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  g_client->onConnectionStatusChanged(event);
}

}  // namespace mondradiko
