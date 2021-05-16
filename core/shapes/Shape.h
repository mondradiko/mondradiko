// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/bullet_headers.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class AabbShape;
class DebugDrawList;

template <typename Impl>
class Shape {
 public:
  using vec3 = glm::dvec3;
  using ivec3 = glm::vec<3, int64_t>;
  using quat = glm::dquat;

  struct Transform {
    vec3 origin;
    quat basis;
  };

  // "Virtual" methods
  static void debugDraw(const Impl*, const glm::mat4&, const glm::vec3&,
                        DebugDrawList*);
  static void getBoundingBox(const Impl*, const Transform*, AabbShape*);
  static btCollisionShape* createBulletShape(const Impl*);
};

}  // namespace core
}  // namespace mondradiko
