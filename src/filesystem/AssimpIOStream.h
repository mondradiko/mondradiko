#pragma once

#include <vector>

#include "api_headers.h"

class Filesystem;

class AssimpIOStream : public Assimp::IOStream
{
public:
    AssimpIOStream(Filesystem*, const char*, const char*);
    ~AssimpIOStream();

    size_t Read(void*, size_t, size_t);
    size_t Write(const void*, size_t, size_t);
    aiReturn Seek(size_t, aiOrigin);
    size_t Tell() const;
    size_t FileSize() const;
    void Flush();
private:
    Filesystem* fs;

    std::vector<char> contents;
    size_t position;
};
