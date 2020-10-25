/**
 * @file Scene.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "scene/Scene.h"

#include <iostream>

#include "filesystem/AssimpIOSystem.h"
#include "filesystem/Filesystem.h"
#include "graphics/Renderer.h"
#include "log/AssimpLogStream.h"
#include "log/log.h"

Scene::Scene(Filesystem* fs, Renderer* renderer)
    : fs(fs), renderer(renderer), rootEntity(this) {
  log_dbg("Creating scene.");

  // Abandon all hope, ye who enter here.
  Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(Log::LOG_LEVEL_INFO), Assimp::Logger::Info);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(Log::LOG_LEVEL_DEBUG), Assimp::Logger::Debugging);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(Log::LOG_LEVEL_WARNING), Assimp::Logger::Warn);
  Assimp::DefaultLogger::get()->attachStream(
      new AssimpLogStream(Log::LOG_LEVEL_ERROR), Assimp::Logger::Err);

  importer.SetIOHandler(new AssimpIOSystem(fs));
}

Scene::~Scene() {
  log_dbg("Destroying scene.");

  Assimp::DefaultLogger::kill();
}

void Scene::update(double dt) {}

bool Scene::loadModel(const char* fileName) {
  // TODO(marceline-cramer) Go into more detail on flags
  const aiScene* modelScene =
      importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Fast);

  if (!modelScene) {
    log_err("Failed to read model scene.");
    return false;
  }

  rootEntity.addChild(
      new Entity(this, fileName, modelScene, modelScene->mRootNode));

  // aiScene is freed automatically
  return true;
}
