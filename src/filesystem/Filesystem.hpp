#pragma once

#include "api_headers.hpp"

class Filesystem
{
public:
    Filesystem(const char*);
    ~Filesystem();

    const char* archive;
private:
};
