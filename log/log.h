// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <cstdio>

#define log_at(logLevel, message) \
  mondradiko::log(__FILE__, __LINE__, logLevel, message)
#define log_inf(message) log_at(mondradiko::LogLevel::Info, message)
#define log_dbg(message) log_at(mondradiko::LogLevel::Debug, message)
#define log_msg(message) log_at(mondradiko::LogLevel::Message, message)
#define log_wrn(message) log_at(mondradiko::LogLevel::Warn, message)
#define log_err(message) log_at(mondradiko::LogLevel::Error, message)
#define log_ftl(message) log_at(mondradiko::LogLevel::Fatal, message)

#define log_at_fmt(logLevel, ...) \
  mondradiko::log_formatted(__FILE__, __LINE__, logLevel, __VA_ARGS__)
#define log_inf_fmt(...) log_at_fmt(mondradiko::LogLevel::Info, __VA_ARGS__)
#define log_dbg_fmt(...) log_at_fmt(mondradiko::LogLevel::Debug, __VA_ARGS__)
#define log_msg_fmt(...) log_at_fmt(mondradiko::LogLevel::Message, __VA_ARGS__)
#define log_wrn_fmt(...) log_at_fmt(mondradiko::LogLevel::Warn, __VA_ARGS__)
#define log_err_fmt(...) log_at_fmt(mondradiko::LogLevel::Error, __VA_ARGS__)
#define log_ftl_fmt(...) log_at_fmt(mondradiko::LogLevel::Fatal, __VA_ARGS__)

// Enable Tracy for Intellisense
#if defined(__INTELLISENSE__) && !defined(TRACY_ENABLE)
#define TRACY_ENABLE
#endif

#ifdef TRACY_ENABLE
#include "lib/tracy/Tracy.hpp"
#define log_zone \
  ZoneScoped;    \
  // mondradiko::log(__FILE__, __LINE__, LogLevel::Zone, __FUNCTION__);
#define log_zone_named(name) \
  ZoneScopedN(name);         \
  // mondradiko::log(__FILE__, __LINE__, LogLevel::Zone, __FUNCTION__);
#define log_frame_mark FrameMark
#else
#define log_zone
#define log_zone_named(name)
#define log_frame_mark
#endif

namespace mondradiko {

enum class LogLevel { Zone = 0, Info, Debug, Message, Warn, Error, Fatal };

const char* getLogPrefix(LogLevel);

void log(const char*, int, LogLevel, const char*);

template <typename... Args>
void log_formatted(const char* file, int line, LogLevel level,
                   const char* format, Args&&... args) {
  char message[512];
  snprintf(message, sizeof(message), format, args...);
  log(file, line, level, message);
}

}  // namespace mondradiko
