/**
 * @file AssetPool.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Loads and maintains Assets by ID.
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <type_traits>
#include <unordered_map>

#include "core/assets/Asset.h"
#include "core/common/api_headers.h"
#include "core/filesystem/Filesystem.h"
#include "log/log.h"

namespace mondradiko {

template <typename BaseAssetType>
struct DummyAsset {
  BaseAssetType* dummy = nullptr;
};

class AssetPool {
 public:
  explicit AssetPool(Filesystem* fs) : fs(fs) {
    template_asset = asset_registry.create();
  }

  template <typename AssetType, typename... Args>
  void initializeAssetType(Args&&... args) {
    auto& asset =
        asset_registry.emplace<AssetType>(template_asset, this, args...);
    asset.loaded = false;
  }

  template <typename AssetType, typename... Args>
  void initializeDummyAssetType(Args&&... args) {
    initializeAssetType<AssetType>(args...);
    asset_registry.emplace<DummyAsset<AssetType>>(template_asset);
  }

  template <typename AssetType>
  AssetId loadAsset(AssetId id) {
    if (!isAssetTypeInitialized<AssetType>()) {
      log_ftl("Attempted to load asset with uninitialized type %s",
              typeid(AssetType).name());
    }

    auto iter = local_ids.find(id);
    if (iter != local_ids.end()) return id;

    AssetId local_id = asset_registry.create(id);
    local_ids.emplace(id, local_id);

    const assets::SerializedAsset* asset_data;
    bool loaded_successfully = fs->loadAsset(&asset_data, id);

    if (loaded_successfully) {
      if (asset_data->type() != AssetType::ASSET_TYPE) {
        loaded_successfully = false;
        log_err("SerializedAsset does not have type %s as expected",
                assets::EnumNameAssetType(AssetType::ASSET_TYPE));
      }
    }

    if (!loaded_successfully) {
      log_err("Failed to load asset 0x%0lx", id);
    }

    AssetType& initial_asset = asset_registry.get<AssetType>(template_asset);
    AssetType& new_asset =
        asset_registry.emplace<AssetType>(local_id, initial_asset);
    new_asset.loaded = false;

    // Dummy assets are never loaded, but are initialized
    if (loaded_successfully && !isAssetTypeDummy<AssetType>()) {
      new_asset.load(asset_data);
      new_asset.loaded = true;
    }

    return id;
  }

  template <typename AssetType>
  bool isAssetTypeInitialized() const {
    return asset_registry.has<AssetType>(template_asset);
  }

  template <typename AssetType>
  bool isAssetTypeDummy() const {
    const DummyAsset<AssetType>* asset_ptr =
        asset_registry.try_get<DummyAsset<AssetType>>(template_asset);
    if (!asset_ptr) return false;
    return true;
  }

  template <typename AssetType>
  AssetType& getAsset(AssetId id) {
    auto iter = local_ids.find(id);

    AssetId local_id;
    if (iter == local_ids.end()) {
      loadAsset<AssetType>(id);
      local_id = local_ids.find(id)->second;
    } else {
      local_id = iter->second;
    }

    return asset_registry.get<AssetType>(local_id);
  }

  template <typename AssetType>
  void unloadAll() {
    auto assets_view = asset_registry.view<AssetType>();
    for (auto local_id : assets_view) {
      auto& asset = assets_view.get(local_id);
      if (asset.isLoaded()) {
        asset.unload();
        asset.loaded = false;
      }
    }
  }

 private:
  Filesystem* fs;

  AssetId template_asset;
  std::unordered_map<AssetId, AssetId> local_ids;
  entt::basic_registry<AssetId> asset_registry;
};

}  // namespace mondradiko
