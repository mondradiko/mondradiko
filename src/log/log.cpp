#include "log/log.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>

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

void Log::log(const char* filePath, int line, LogLevel level, const char* message)
{
    const char* prefix = getLogPrefix(level);
    std::ostringstream header;
    std::string fileName(filePath);

    // Remove "../src/" from file path
    header << prefix << fileName.substr(7) << ":" << line << "]";

    std::cerr << std::left << std::setw(55) << header.str();
    std::cerr << message << "\e[0m" << std::endl;

    if(level == Log::LOG_LEVEL_FATAL) {
        throw std::runtime_error(message);
    }
}
