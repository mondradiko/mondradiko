/**
 * @file log.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides global logging with formatting.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "log/log.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace mondradiko {

const std::filesystem::path g_root_path =
    std::filesystem::path(__FILE__).parent_path().parent_path();

std::string formatPath(const std::filesystem::path& file_path) {
  return file_path.lexically_relative(g_root_path);
}

const char* getLogPrefix(LogLevel level) {
  switch (level) {
    case LOG_LEVEL_ZONE:
      return "\e[33m[ZNE ";
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

void log(const char* file_path, int line, LogLevel level, const char* message) {
  const char* prefix = getLogPrefix(level);
  std::ostringstream header;
  header << prefix << formatPath(file_path) << ":" << line << "]";

  std::cerr << std::left << std::setw(55) << header.str();
  std::cerr << message << "\e[0m" << std::endl;

  if (level == LOG_LEVEL_FATAL) {
    throw std::runtime_error(message);
  }
}

}  // namespace mondradiko
