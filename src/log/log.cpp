#include "log/log.hpp"

#include <iostream>
#include <iomanip>

const char* Log::getLogPrefix(LogLevel level)
{
    switch(level) {
    case Log::LOG_LEVEL_INFO:
        return "\e[36m[INF ";
    case Log::LOG_LEVEL_DEBUG:
        return "\e[32m[DBG ";
    case Log::LOG_LEVEL_WARNING:
        return "\e[33m[WRN ";
    case Log::LOG_LEVEL_ERROR:
        return "\e[33m[ERR ";
    default:
        return "\e[31m[FTL ";
    }
}

void Log::log(const char* file, int line, LogLevel level, const char* message)
{
    const char* prefix = getLogPrefix(level);
    std::ostringstream header;
    header << prefix << file << ":" << line << "]";

    std::cerr << std::left << std::setw(55) << header.str();
    std::cerr << message << "\e[0m" << std::endl;
}
