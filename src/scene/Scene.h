/**
 * @file Scene.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
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
