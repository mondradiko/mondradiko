/**
 * @file log.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides global logging with formatting.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "log/log.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace mondradiko {

const char* getLogPrefix(LogLevel level) {
  switch (level) {
    case LOG_LEVEL_INFO:
      return "\e[36m[INF ";
    case LOG_LEVEL_DEBUG:
      return "\e[32m[DBG ";
    case LOG_LEVEL_WARNING:
      return "\e[33m[WRN ";
    case LOG_LEVEL_ERROR:
      return "\e[33m[ERR ";
    default:
      return "\e[31m[FTL ";
  }
}

void log(const char* file_path, int line, LogLevel level,
              const char* message) {
  const char* prefix = getLogPrefix(level);
  std::ostringstream header;
  std::string filename(file_path);

  // Remove "../src/" from file path
  header << prefix << filename.substr(7) << ":" << line << "]";

  std::cerr << std::left << std::setw(55) << header.str();
  std::cerr << message << "\e[0m" << std::endl;

  if (level == LOG_LEVEL_FATAL) {
    throw std::runtime_error(message);
  }
}

}  // namespace mondradiko
