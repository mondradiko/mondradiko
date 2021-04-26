// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"
#include "core/shapes/AnyShape.h"

namespace mondradiko {
namespace core {

class ShapeAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::ShapeAsset);

  ShapeAsset() {}
  ~ShapeAsset();

  btCollisionShape* getCollisionShape() const { return _collision_shape; }

 protected:
  // Asset implementation
  bool _load(const assets::SerializedAsset*) final;

 private:
  AnyShape _shape;
  btCollisionShape* _collision_shape = nullptr;
};

}  // namespace core
}  // namespace mondradiko
