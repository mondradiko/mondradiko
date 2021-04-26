// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetHandle.h"
#include "core/assets/AssetPool.h"
#include "core/assets/ShapeAsset.h"
#include "core/components/SynchronizedComponent.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/ShapeComponent_generated.h"

namespace mondradiko {
namespace core {

class ShapeComponent : public SynchronizedComponent<protocol::ShapeComponent> {
 public:
  explicit ShapeComponent(const protocol::ShapeComponent& data)
      : SynchronizedComponent(data) {}

  explicit ShapeComponent(const assets::ShapePrefab* prefab) {
    _data.mutate_shape_asset(prefab->shape_asset());
  }

  // Component implementation
  void refresh(AssetPool* asset_pool) {
    _shape = asset_pool->load<ShapeAsset>(_data.shape_asset());
  }

  bool isLoaded() const { return _shape.isLoaded(); }

  const AssetHandle<ShapeAsset>& getShape() { return _shape; }

 private:
  AssetHandle<ShapeAsset> _shape;
};

}  // namespace core
}  // namespace mondradiko
