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

#include "components/Component.h"
#include "components/MeshRendererComponent.h"
#include "graphics/Renderer.h"
#include "log/log.h"
#include "scene/Scene.h"

Entity::Entity(Scene* scene) : name("root"), scene(scene) {}

Entity::Entity(Scene* scene, std::string parentName, const aiScene* modelScene,
               aiNode* node)
    : name(node->mName.C_Str()), scene(scene) {
  std::ostringstream nodePathFormat;
  nodePathFormat << parentName << "/" << node->mName.C_Str();
  std::string nodePath = nodePathFormat.str();

  log_inf("Loading entity:");
  log_inf(nodePath.c_str());

  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    uint32_t meshIndex = node->mMeshes[i];
    addComponent(scene->renderer->meshPipeline->createMeshRenderer(
        parentName, modelScene, meshIndex));
  }

  // Recursively add subnodes
  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    addChild(new Entity(scene, nodePath, modelScene, node));
  }
}

Entity::~Entity() {
  log_inf("Destroying entity:");
  log_inf(name.c_str());

  while (child) delete child;

  if (parent) parent->child = nextSibling;
  if (prevSibling) prevSibling->nextSibling = nextSibling;
  if (nextSibling) nextSibling->prevSibling = prevSibling;

  while (firstComponent) delete firstComponent;
}

void Entity::addChild(Entity* newChild) {
  newChild->parent = this;
  newChild->nextSibling = child;
  if (child) child->prevSibling = newChild;
  child = newChild;
}

void Entity::addComponent(Component* component) {
  component->parent = this;
  component->next = firstComponent;
  firstComponent = component;
}
