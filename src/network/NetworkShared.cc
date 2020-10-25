/**
 * @file NetworkShared.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides shared functionality between client and server derived
 * classes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "network/NetworkShared.h"

#include "log/log.h"

NetworkShared::~NetworkShared() { log_dbg("Disconnecting from server."); }

bool NetworkShared::connect(const char* serverAddress, int port) {
  log_dbg("Connecting to server.");

  return true;
}

bool NetworkShared::authenticate() {
  log_dbg("Authenticating client identity.");

  return true;
}