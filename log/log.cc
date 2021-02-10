// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "log/log.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

namespace mondradiko {

const std::filesystem::path g_root_path =
    std::filesystem::path(__FILE__).parent_path().parent_path();

std::string formatPath(const std::filesystem::path& file_path) {
  return file_path.lexically_relative(g_root_path).string();
}

const char* getLogPrefix(LogLevel level) {
  switch (level) {
    case LOG_LEVEL_ZONE:
      return "\x1b[33m[ZNE ";
    case LOG_LEVEL_INFO:
      return "\x1b[36m[INF ";
    case LOG_LEVEL_DEBUG:
      return "\x1b[32m[DBG ";
    case LOG_LEVEL_WARNING:
      return "\x1b[33m[WRN ";
    case LOG_LEVEL_ERROR:
      return "\x1b[33m[ERR ";
    default:
      return "\x1b[31m[FTL ";
  }
}

void log(const char* file_path, int line, LogLevel level, const char* message) {
  const char* prefix = getLogPrefix(level);
  std::ostringstream header;
  header << prefix << formatPath(file_path) << ":" << line << "]";

  std::cerr << std::left << std::setw(55) << header.str();
  std::cerr << message << "\x1b[0m" << std::endl;

  if (level == LOG_LEVEL_FATAL) {
    throw std::runtime_error(message);
  }
}

}  // namespace mondradiko
