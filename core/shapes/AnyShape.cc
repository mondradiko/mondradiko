// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/shapes/AnyShape.h"

#include "log/log.h"

namespace mondradiko {
namespace core {

template <>
void Shape<AnyShape>::debugDraw(const AnyShape* self,
                                const glm::mat4& transform,
                                const glm::vec3& color,
                                DebugDrawList* debug_draw) {
  auto& shape = self->_shape;

  switch (self->_shape_type) {
    case ShapeType::Sphere: {
      SphereShape::debugDraw(&shape.sphere, transform, color, debug_draw);
      return;
    }

    default: {
      log_err("ShapeType has not implemented debugDraw()");
      return;
    }
  }
}

template <>
btCollisionShape* Shape<AnyShape>::createBulletShape(const AnyShape* self) {
  auto& shape = self->_shape;

  switch (self->_shape_type) {
    case ShapeType::Sphere:
      return SphereShape::createBulletShape(&shape.sphere);

    default: {
      log_err("ShapeType is not collidable");
      return nullptr;
    }
  }
}

}  // namespace core
}  // namespace mondradiko
