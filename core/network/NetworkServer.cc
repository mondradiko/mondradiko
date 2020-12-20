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

#include "core/scene/Scene.h"
#include "log/log.h"
#include "steam/steamnetworkingsockets.h"

namespace mondradiko {

NetworkServer::NetworkServer(Scene* scene, const char* server_address,
                             int server_port)
    : scene(scene) {
  log_zone;

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

  sockets->CreateListenSocketIP(address, 0, nullptr);
}

NetworkServer::~NetworkServer() { log_zone; }

}  // namespace mondradiko
