// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/shapes/SphereShape.h"

#include "log/log.h"

namespace mondradiko {
namespace core {

template <>
btCollisionShape* Shape<SphereShape>::createBulletShape(
    const SphereShape* self) {
  return new btSphereShape(self->radius);
}

}  // namespace core
}  // namespace mondradiko
