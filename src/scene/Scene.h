/**
 * @file Scene.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Scene configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_SCENE_SCENE_H_
#define SRC_SCENE_SCENE_H_

#include <map>

#include "filesystem/Filesystem.h"
#include "graphics/Renderer.h"
#include "scene/Entity.h"
#include "src/api_headers.h"

class Scene {
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

#endif  // SRC_SCENE_SCENE_H_
