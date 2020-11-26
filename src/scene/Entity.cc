/**
 * @file Entity.cc
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

#include "scene/Entity.h"

#include <sstream>

#include "components/Component.h"
#include "components/MeshRendererComponent.h"
#include "log/log.h"
#include "renderer/Renderer.h"
#include "scene/Scene.h"

namespace mondradiko {

Entity::Entity(Scene* scene) : name("root"), scene(scene) {}

Entity::Entity(Scene* scene, std::string parent_name,
               const aiScene* model_scene, aiNode* node)
    : name(node->mName.C_Str()), scene(scene) {
  std::ostringstream nodePathFormat;
  nodePathFormat << parent_name << "/" << node->mName.C_Str();
  std::string nodePath = nodePathFormat.str();

  log_inf("Loading entity:");
  log_inf(nodePath.c_str());

  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    uint32_t meshIndex = node->mMeshes[i];
    addComponent(scene->renderer->mesh_pipeline->createMeshRenderer(
        parent_name, model_scene, meshIndex));
  }

  // Recursively add subnodes
  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    addChild(new Entity(scene, nodePath, model_scene, node));
  }
}

Entity::~Entity() {
  log_inf("Destroying entity:");
  log_inf(name.c_str());

  while (child) delete child;

  if (parent) parent->child = next_sibling;
  if (prev_sibling) prev_sibling->next_sibling = next_sibling;
  if (next_sibling) next_sibling->prev_sibling = prev_sibling;

  while (first_component) delete first_component;
}

void Entity::addChild(Entity* new_child) {
  new_child->parent = this;
  new_child->next_sibling = child;
  if (child) child->prev_sibling = new_child;
  child = new_child;
}

void Entity::addComponent(Component* new_component) {
  new_component->parent = this;
  new_component->next = first_component;
  first_component = new_component;
}

}  // namespace mondradiko
