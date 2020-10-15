#pragma once

#include "api_headers.hpp"

class Filesystem;

class Scene
{
public:
    Scene(Filesystem*);
    ~Scene();

    void update(double);

    bool loadModel(const char*);
private:
    Filesystem* fs;

    Assimp::Importer importer;
};
