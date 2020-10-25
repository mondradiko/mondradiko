/**
 * @file log.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides global logging with formatting.
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

#ifndef SRC_LOG_LOG_H_
#define SRC_LOG_LOG_H_

#define log_at(logLevel, ...) \
  Log::log(__FILE__, __LINE__, logLevel, __VA_ARGS__)
#define log_inf(...) log_at(Log::LOG_LEVEL_INFO, __VA_ARGS__)
#define log_dbg(...) log_at(Log::LOG_LEVEL_DEBUG, __VA_ARGS__)
#define log_wrn(...) log_at(Log::LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_err(...) log_at(Log::LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_ftl(...) log_at(Log::LOG_LEVEL_FATAL, __VA_ARGS__)

namespace Log {
enum LogLevel {
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL
};

const char* getLogPrefix(LogLevel);
void log(const char*, int, LogLevel, const char*);
};  // namespace Log

#endif  // SRC_LOG_LOG_H_
