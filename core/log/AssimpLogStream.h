/**
 * @file AssimpLogStream.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits Assimp::LogStream to provide logging functionality to Assimp.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/log/log.h"
#include "core/common/api_headers.h"

namespace mondradiko {

// I am so sorry to whoever reading this
// that this class has to exist and that
// it exists in the way that it does,
// but Assimp doesn't really give me
// much of a choice here.

class AssimpLogStream : public Assimp::LogStream {
 public:
  explicit AssimpLogStream(LogLevel log_level) : log_level(log_level) {}

  void write(const char* message) { log_at(log_level, message); }

 private:
  LogLevel log_level;
};

}  // namespace mondradiko
