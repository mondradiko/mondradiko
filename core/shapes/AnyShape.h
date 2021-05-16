// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/shapes/AabbShape.h"
#include "core/shapes/FrustumShape.h"
#include "core/shapes/IntAabbShape.h"
#include "core/shapes/Shape.h"
#include "core/shapes/SphereShape.h"

namespace mondradiko {
namespace core {

enum ShapeType { None = 0, Aabb, Frustum, IntAabb, Sphere };

class AnyShape : public Shape<AnyShape> {
 public:
  AnyShape() {}
  AnyShape(const AabbShape& shape) { loadAabb(shape); }
  AnyShape(const FrustumShape& shape) { loadFrustum(shape); }
  AnyShape(const IntAabbShape& shape) { loadIntAabb(shape); }
  AnyShape(const SphereShape& shape) { loadSphere(shape); }

  void loadAabb(const AabbShape& shape) {
    _shape_type = ShapeType::Aabb;
    _shape.aabb = shape;
  }

  void loadFrustum(const FrustumShape& shape) {
    _shape_type = ShapeType::Frustum;
    _shape.frustum = shape;
  }

  void loadIntAabb(const IntAabbShape& shape) {
    _shape_type = ShapeType::IntAabb;
    _shape.int_aabb = shape;
  }

  void loadSphere(const SphereShape& shape) {
    _shape_type = ShapeType::Sphere;
    _shape.sphere = shape;
  }

 private:
  friend class Shape;

  ShapeType _shape_type = None;

  union {
    AabbShape aabb;
    FrustumShape frustum;
    IntAabbShape int_aabb;
    SphereShape sphere;
  } _shape;
};

}  // namespace core
}  // namespace mondradiko
