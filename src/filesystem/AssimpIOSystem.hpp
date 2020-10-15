#pragma once

#include "api_headers.hpp"

class Filesystem;

class AssimpIOSystem : public Assimp::IOSystem
{
public:
    AssimpIOSystem(Filesystem*);
    ~AssimpIOSystem();

    bool Exists(const char*) const;
    char getOsSeparator() const { return '/'; }
    Assimp::IOStream* Open(const char*, const char*);
    void Close(Assimp::IOStream*);
private:
    Filesystem* fs;
};
