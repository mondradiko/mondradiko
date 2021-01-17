/**
 * @file PrefabAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates new entities with components as saved in an asset.
 * @date 2021-14-01
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/format/PrefabAsset_generated.h"
#include "core/assets/Asset.h"
#include "core/world/Entity.h"

namespace mondradiko {

// Forward declarations
class AssetPool;
class TransformComponent;

class PrefabAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::PrefabAsset);

  // Asset lifetime implementation
  PrefabAsset(AssetPool* asset_pool) : asset_pool(asset_pool) {}
  void load(const assets::SerializedAsset*) final;
  ~PrefabAsset();

  EntityId instantiate(EntityRegistry*, const TransformComponent&) const;

 private:
  AssetPool* asset_pool;

  assets::PrefabAssetT* prefab;
};

}  // namespace mondradiko
