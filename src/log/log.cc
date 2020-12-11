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

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace mondradiko {

/**
 * Find the number of characters required to trim out
 * the relative __FILE__ paths.
 *
 * First, take this file. (src/log/log.cc)
 * Take the parent directory. (src/log/)
 * Take the next parent directory. (src/)
 *
 * This string represents the filler directories that
 * are generated in the build system and passed to
 * __FILE__, so we save the length of it and use it
 * to trim out directories in log().
 *
 */
const size_t file_trim = std::filesystem::path(__FILE__)
                             .parent_path()
                             .parent_path()
                             .string()
                             .length();

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
  std::string filename(file_path);

  // Trim out relative source path
  header << prefix << filename.substr(file_trim) << ":" << line << "]";

  std::cerr << std::left << std::setw(55) << header.str();
  std::cerr << message << "\e[0m" << std::endl;

  if (level == LOG_LEVEL_FATAL) {
    throw std::runtime_error(message);
  }
}

}  // namespace mondradiko
