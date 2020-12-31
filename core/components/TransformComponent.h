/**
 * @file TransformComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Transforms an entity and its children.
 * @date 2020-12-16
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/common/glm_headers.h"
#include "core/components/Component.h"
#include "core/world/Entity.h"
#include "protocol/TransformComponent_generated.h"

namespace mondradiko {

class TransformComponent : public Component<protocol::TransformComponent> {
 public:
  TransformComponent() : Component() {}
  explicit TransformComponent(const protocol::TransformComponent& data)
      : Component(data) {}

  // Local parent ID
  EntityId local_parent;
  // Used to sort by parent
  EntityId this_entity;
  // Final transform result used in math
  glm::mat4 world_transform;

  // Used by World to calculate transforms
  glm::mat4 getLocalTransform();
};

}  // namespace mondradiko
