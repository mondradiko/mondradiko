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

#ifndef SRC_SCENE_SCENE_H_
#define SRC_SCENE_SCENE_H_

#include <map>

#include "filesystem/Filesystem.h"
#include "renderer/Renderer.h"
#include "scene/Entity.h"
#include "src/api_headers.h"

namespace mondradiko {

class Scene {
 public:
  Scene(Filesystem*, Renderer*);
  ~Scene();

  void update(double);

  bool loadModel(const char*);

  Filesystem* fs;
  Renderer* renderer;

 private:
  Entity root_entity;

  Assimp::Importer importer;
};

}  // namespace mondradiko

#endif  // SRC_SCENE_SCENE_H_
