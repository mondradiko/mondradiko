/**
 * @file NetworkShared.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides shared functionality between client and server derived
 * classes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/network/NetworkShared.h"

#include "log/log.h"

namespace mondradiko {

NetworkShared::~NetworkShared() { log_dbg("Disconnecting from server."); }

bool NetworkShared::connect(const char* server_address, int port) {
  log_dbg("Connecting to server.");

  return true;
}

bool NetworkShared::authenticate() {
  log_dbg("Authenticating client identity.");

  return true;
}

}  // namespace mondradiko
