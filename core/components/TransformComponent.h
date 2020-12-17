/**
 * @file TransformComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Transforms an entity and its children.
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#pragma once

#include "core/components/Component.h"

namespace mondradiko {

struct TransformComponent {
  EntityId parent;

  glm::mat4 local_transform;
  glm::mat4 world_transform;
};

}  // namespace mondradiko
