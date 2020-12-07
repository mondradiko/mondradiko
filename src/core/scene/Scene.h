/**
 * @file Scene.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <map>

#include "filesystem/Filesystem.h"
#include "renderer/Renderer.h"

namespace mondradiko {

class Scene {
 public:
  Scene(Filesystem*, Renderer*);
  ~Scene();

  void update(double);

  bool loadModel(const char*);

  Filesystem* fs;
  Renderer* renderer;

  // TODO(marceline-cramer) Move session lifetime to Scene
  entt::registry registry;

 private:
  Assimp::Importer importer;
};

}  // namespace mondradiko
