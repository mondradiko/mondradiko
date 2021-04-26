// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/shapes/SphereShape.h"

#include "core/renderer/DebugDrawList.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

template <>
void Shape<SphereShape>::debugDraw(const SphereShape* self,
                                   const glm::mat4& transform,
                                   const glm::vec3& color,
                                   DebugDrawList* debug_draw) {
  glm::vec3 center = transform * glm::vec4(0.0, 0.0, 0.0, 1.0);
  debug_draw->drawIcosahedron(center, self->radius, color);
}

template <>
btCollisionShape* Shape<SphereShape>::createBulletShape(
    const SphereShape* self) {
  return new btSphereShape(self->radius);
}

}  // namespace core
}  // namespace mondradiko
