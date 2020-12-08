/**
 * @file NetworkServer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Hosts a port and communicates to connecting clients with scene events
 * and server metadata.
 * @date 2020-12-06
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "scene/Scene.h"
#include "network/NetworkShared.h"

namespace mondradiko {

class NetworkServer : public NetworkShared {
 public:
  NetworkServer(Scene*, const char*, int);
  ~NetworkServer();
 private:
  Scene* scene;
};

}  // namespace mondradiko
