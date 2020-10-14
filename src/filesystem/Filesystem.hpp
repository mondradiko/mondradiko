#pragma once

#include <vector>

#include "api_headers.hpp"

class Filesystem
{
public:
    Filesystem(const char*);
    ~Filesystem();

    const char* archive;

    bool loadBinaryFile(const char*, std::vector<char>*);
private:
};
