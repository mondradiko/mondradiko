#include "scene/Scene.hpp"

#include <iostream>

#include "filesystem/AssimpIOSystem.hpp"
#include "filesystem/Filesystem.hpp"
#include "graphics/Renderer.hpp"
#include "log/AssimpLogStream.hpp"
#include "log/log.hpp"

Scene::Scene(Filesystem* fs, Renderer* renderer)
 : fs(fs),
   renderer(renderer)
{
    log_dbg("Creating scene.");

    // Abandon all hope, ye who enter here.
    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
    Assimp::DefaultLogger::get()->attachStream(new AssimpLogStream(Log::LOG_LEVEL_INFO), Assimp::Logger::Info);
    Assimp::DefaultLogger::get()->attachStream(new AssimpLogStream(Log::LOG_LEVEL_DEBUG), Assimp::Logger::Debugging);
    Assimp::DefaultLogger::get()->attachStream(new AssimpLogStream(Log::LOG_LEVEL_WARNING), Assimp::Logger::Warn);
    Assimp::DefaultLogger::get()->attachStream(new AssimpLogStream(Log::LOG_LEVEL_ERROR), Assimp::Logger::Err);

    importer.SetIOHandler(new AssimpIOSystem(fs));
}

Scene::~Scene()
{
    log_dbg("Destroying scene.");

    Assimp::DefaultLogger::kill();
}

void Scene::update(double dt)
{

}

bool Scene::loadModel(const char* fileName)
{
    const aiScene* modelScene = importer.ReadFile(fileName, 0);

    if(!modelScene) {
        log_err("Failed to read model scene.");
        return false;
    }

    for(uint32_t i = 0; i < modelScene->mNumMeshes; i++) {
        renderer->meshPipeline->loadMesh(fileName, modelScene->mMeshes[i]);
    }

    // aiScene is freed automatically
    return true;
}
