// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/shapes/AnyShape.h"

#include "log/log.h"

namespace mondradiko {
namespace core {

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
