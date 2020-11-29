/**
 * @file log.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides global logging with formatting.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "api_headers.h"

#ifndef SRC_LOG_LOG_H_
#define SRC_LOG_LOG_H_

#define log_at(logLevel, ...) \
  mondradiko::log(__FILE__, __LINE__, logLevel, __VA_ARGS__)
#define log_inf(...) log_at(mondradiko::LOG_LEVEL_INFO, __VA_ARGS__)
#define log_dbg(...) log_at(mondradiko::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define log_wrn(...) log_at(mondradiko::LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_err(...) log_at(mondradiko::LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_ftl(...) log_at(mondradiko::LOG_LEVEL_FATAL, __VA_ARGS__)
#define log_zone \
  ZoneScoped;    \
  mondradiko::log(__FILE__, __LINE__, LOG_LEVEL_ZONE, __FUNCTION__);

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

}  // namespace mondradiko

#endif  // SRC_LOG_LOG_H_
