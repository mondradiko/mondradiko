#include "scene/Scene.h"

#include <iostream>

#include "filesystem/AssimpIOSystem.h"
#include "filesystem/Filesystem.h"
#include "graphics/Renderer.h"
#include "log/AssimpLogStream.h"
#include "log/log.h"

Scene::Scene(Filesystem* fs, Renderer* renderer)
 : fs(fs),
   renderer(renderer),
   rootEntity(this)
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
    // TODO Go into more detail on flags
    const aiScene* modelScene = importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Fast);

    if(!modelScene) {
        log_err("Failed to read model scene.");
        return false;
    }

    rootEntity.addChild(new Entity(this, fileName, modelScene, modelScene->mRootNode));

    // aiScene is freed automatically
    return true;
}
