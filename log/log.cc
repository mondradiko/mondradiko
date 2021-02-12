// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "log/log.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace mondradiko {

const std::filesystem::path g_root_path =
    std::filesystem::path(__FILE__).parent_path().parent_path();

std::string formatPath(const std::filesystem::path& file_path) {
  return file_path.lexically_relative(g_root_path).string();
}

const char* getLogPrefix(LogLevel level) {
  switch (level) {
    case LogLevel::Zone:
      return "\x1b[33m[ZNE ";
    case LogLevel::Info:
      return "\x1b[94m[INF ";
    case LogLevel::Debug:
      return "\x1b[32m[DBG ";
    case LogLevel::Message:
      return "\x1b[96m[MSG ";
    case LogLevel::Warn:
      return "\x1b[33m[WRN ";
    case LogLevel::Error:
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

  if (level == LogLevel::Fatal) {
    throw std::runtime_error(message);
  }
}

}  // namespace mondradiko
