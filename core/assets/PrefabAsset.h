// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetHandle.h"
#include "core/world/Entity.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class AssetPool;
class ScriptEnvironment;
class TransformComponent;
class World;

class PrefabAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::PrefabAsset);

  // Asset lifetime implementation
  explicit PrefabAsset(AssetPool* asset_pool) : asset_pool(asset_pool) {}
  ~PrefabAsset();

  EntityId instantiate(ScriptEnvironment*, World*) const;

 protected:
  // Asset implementation
  bool _load(const assets::SerializedAsset*) final;

 private:
  AssetPool* asset_pool;

  assets::PrefabAssetT* prefab;
  types::vector<AssetHandle<PrefabAsset>> children;
};

}  // namespace core
}  // namespace mondradiko
