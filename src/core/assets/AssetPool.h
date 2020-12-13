/**
 * @file AssetPool.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Loads and maintains Assets by ID.
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <type_traits>
#include <unordered_map>

#include "assets/loading/ImmutableAsset.h"
#include "core/assets/Asset.h"
#include "core/filesystem/Filesystem.h"
#include "log/log.h"

namespace mondradiko {

class AssetPool {
 public:
  explicit AssetPool(Filesystem* fs) : fs(fs) {}

  template <typename AssetType, typename... Args>
  [[nodiscard]] AssetId loadAsset(AssetId id, Args&&... args) {
    assets::ImmutableAsset asset_data;
    fs->loadAsset(&asset_data, id);

    AssetId asset = asset_registry.create(id);
    
    asset_registry.emplace<AssetType>(asset, asset_data, std::forward<Args>(args)...);
    asset_registry.get<AssetType>(asset).loaded = true;
    return asset;
  }

  template <typename AssetType>
  bool isAssetLoaded(AssetId id) const {
    if (!asset_registry.valid(id)) return false;

    auto asset = asset_registry.try_get<AssetType>(id);

    if (asset) {
      return asset->loaded;
    } else {
      return false;
    }
  }

  template <typename AssetType>
  const AssetType& getAsset(AssetId id) const {
    return asset_registry.get<AssetType>(id);
  }

 private:
  Filesystem* fs;

  entt::basic_registry<AssetId> asset_registry;
};

}  // namespace mondradiko
