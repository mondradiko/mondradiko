#pragma once

#include <map>

#include "api_headers.h"

#include "scene/Entity.h"

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
