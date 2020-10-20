#pragma once

#include "api_headers.h"

#include "log/log.h"

// I am so sorry to whoever reading this
// that this class has to exist and that
// it exists in the way that it does,
// but Assimp doesn't really give me
// much of a choice here.

class AssimpLogStream : public Assimp::LogStream
{
public:
    AssimpLogStream(Log::LogLevel logLevel) : logLevel(logLevel) {}

    void write(const char* message) { log_at(logLevel, message); }
private:
    Log::LogLevel logLevel;
};
