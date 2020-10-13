#include "scene/Scene.hpp"

#include "filesystem/Filesystem.hpp"

Scene::Scene(Filesystem* fs)
 : fs(fs)
{

}

Scene::~Scene()
{

}

void Scene::update(double dt)
{

}

bool Scene::loadModel(const char* fileName)
{
    return true;
}
