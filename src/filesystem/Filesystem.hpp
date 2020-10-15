#pragma once

#include <vector>

#include "api_headers.hpp"

class Filesystem
{
public:
    Filesystem(const char*);
    ~Filesystem();

    const char* archive;

    bool exists(const char*);
    bool loadBinaryFile(const char*, std::vector<char>*);
private:
};
