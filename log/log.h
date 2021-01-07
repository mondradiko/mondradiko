/**
 * @file log.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides global logging with formatting.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <cstdio>

// Enable Tracy for Intellisense
#if defined(__INTELLISENSE__) && !defined(TRACY_ENABLE)
#define TRACY_ENABLE
#endif

#include "lib/tracy/Tracy.hpp"

#define log_at(logLevel, ...) \
  mondradiko::log_formatted(__FILE__, __LINE__, logLevel, __VA_ARGS__)
#define log_inf(...) log_at(mondradiko::LOG_LEVEL_INFO, __VA_ARGS__)
#define log_dbg(...) log_at(mondradiko::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define log_wrn(...) log_at(mondradiko::LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_err(...) log_at(mondradiko::LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_ftl(...) log_at(mondradiko::LOG_LEVEL_FATAL, __VA_ARGS__)
#define log_zone \
  ZoneScoped;    \
  // mondradiko::log(__FILE__, __LINE__, LOG_LEVEL_ZONE, __FUNCTION__);
#define log_zone_named(name) \
  ZoneScopedN(name);         \
  // mondradiko::log(__FILE__, __LINE__, LOG_LEVEL_ZONE, __FUNCTION__);

namespace mondradiko {

enum LogLevel {
  LOG_LEVEL_ZONE,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL
};

const char* getLogPrefix(LogLevel);

void log(const char*, int, LogLevel, const char*);

template <typename... Args>
void log_formatted(const char* file, int line, LogLevel level,
                   const char* format, Args&&... args) {
  char message[100];
  snprintf(message, sizeof(message), format, args...);
  log(file, line, level, message);
}

}  // namespace mondradiko
