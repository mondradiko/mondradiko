/**
 * @file Scene.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "scene/Scene.h"

#include <iostream>

#include "filesystem/AssimpIOSystem.h"
#include "filesystem/Filesystem.h"
#include "log/AssimpLogStream.h"
#include "log/log.h"

namespace mondradiko {

Scene::Scene(Filesystem* fs, Renderer* renderer) : fs(fs), renderer(renderer) {
  log_zone;

  // Abandon all hope, ye who enter here.
  Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(LOG_LEVEL_INFO), Assimp::Logger::Info);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(LOG_LEVEL_DEBUG), Assimp::Logger::Debugging);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(LOG_LEVEL_WARNING), Assimp::Logger::Warn);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(LOG_LEVEL_ERROR), Assimp::Logger::Err);

  importer.SetIOHandler(new AssimpIOSystem(fs));
}

Scene::~Scene() {
  log_zone;

  Assimp::DefaultLogger::kill();
}

void Scene::update(double dt) {}

bool Scene::loadModel(const char* fileName) {
  log_zone;

  // TODO(marceline-cramer) Go into more detail on flags
  const aiScene* modelScene =
      importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Fast);

  if (!modelScene) {
    log_err("Failed to read model scene.");
    return false;
  }

  aiNode* root_node = modelScene->mRootNode;
  auto node_entity = registry.create();

  if (root_node->mNumChildren > 0) {
    log_err("Child nodes are unsupported for now");
  }

  for (uint32_t i = 0; i < root_node->mNumMeshes; i++) {
    uint32_t mesh_index = root_node->mMeshes[i];
    uint32_t material_index = modelScene->mMeshes[mesh_index]->mMaterialIndex;

    MeshRendererComponent& mesh_renderer_component =
        registry.emplace<MeshRendererComponent>(node_entity);
    mesh_renderer_component.mesh_asset =
        renderer->loadMesh(fileName, modelScene, mesh_index);
    mesh_renderer_component.material_asset =
        renderer->loadMaterial(fileName, modelScene, material_index);
  }

  // aiScene is freed automatically
  return true;
}

}  // namespace mondradiko
