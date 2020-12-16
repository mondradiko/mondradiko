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

namespace mondradiko {

NetworkServer::NetworkServer(Scene* scene, const char* server_address,
                             int server_port)
    : scene(scene) {
  log_zone;
}

NetworkServer::~NetworkServer() { log_zone; }

}  // namespace mondradiko
