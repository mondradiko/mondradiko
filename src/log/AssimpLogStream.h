/**
 * @file AssimpLogStream.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits Assimp::LogStream to provide logging functionality to Assimp.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_LOG_ASSIMPLOGSTREAM_H_
#define SRC_LOG_ASSIMPLOGSTREAM_H_

#include "log/log.h"
#include "src/api_headers.h"

// I am so sorry to whoever reading this
// that this class has to exist and that
// it exists in the way that it does,
// but Assimp doesn't really give me
// much of a choice here.

class AssimpLogStream : public Assimp::LogStream {
 public:
  explicit AssimpLogStream(Log::LogLevel logLevel) : logLevel(logLevel) {}

  void write(const char* message) { log_at(logLevel, message); }

 private:
  Log::LogLevel logLevel;
};

#endif  // SRC_LOG_ASSIMPLOGSTREAM_H_
