// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/ShapeAsset.h"

namespace mondradiko {
namespace core {

ShapeAsset::~ShapeAsset() {
  if (_collision_shape != nullptr) delete _collision_shape;
}

bool ShapeAsset::_load(const assets::SerializedAsset* asset) {
  const assets::ShapeAsset* shape = asset->shape();

  switch (shape->type()) {
    case assets::ShapeType::Sphere: {
      SphereShape sphere;
      sphere.radius = shape->sphere()->radius();
      _shape.loadSphere(sphere);
      break;
    }

    default: {
      log_err("Invalid ShapeType");
      return false;
    }
  }

  _collision_shape = AnyShape::createBulletShape(&_shape);

  return _collision_shape != nullptr;
}

}  // namespace core
}  // namespace mondradiko
