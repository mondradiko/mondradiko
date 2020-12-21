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
