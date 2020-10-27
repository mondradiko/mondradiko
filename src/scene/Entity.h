/**
 * @file Entity.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains Components and child Entities.
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

#ifndef SRC_SCENE_ENTITY_H_
#define SRC_SCENE_ENTITY_H_

#include <string>

#include "components/Component.h"
#include "src/api_headers.h"

class Scene;  // Forward declaration because of codependence

// This is probably the least thread-friendly class in existence
class Entity {
 public:
  explicit Entity(Scene*);
  Entity(Scene*, std::string, const aiScene*, aiNode*);
  ~Entity();

  void addChild(Entity*);
  void addComponent(Component*);

  std::string name;
  Entity* parent = nullptr;
  Entity* child = nullptr;
  Entity* prevSibling = nullptr;
  Entity* nextSibling = nullptr;

  Component* firstComponent = nullptr;

 private:
  Scene* scene;
};

#endif  // SRC_SCENE_ENTITY_H_
