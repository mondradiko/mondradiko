#pragma once

#define log_at(logLevel, ...) Log::log(__FILE__, __LINE__, logLevel, __VA_ARGS__)
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
};
