#pragma once

#include <map>

#include "api_headers.hpp"

#include "scene/Entity.hpp"

class Filesystem;
class Renderer;

class Scene
{
public:
    Scene(Filesystem*, Renderer*);
    ~Scene();

    void update(double);

    bool loadModel(const char*);

    Filesystem* fs;
    Renderer* renderer;
private:
    Entity rootEntity;

    Assimp::Importer importer;
};
