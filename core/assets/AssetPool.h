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
#include <vector>

#include "core/assets/Asset.h"
#include "core/assets/AssetHandle.h"
#include "core/filesystem/Filesystem.h"
#include "lib/include/entt_headers.h"
#include "log/log.h"

namespace mondradiko {

template <typename BaseAssetType>
struct DummyAsset {
  BaseAssetType* dummy = nullptr;
};

class AssetPool {
 public:
  explicit AssetPool(Filesystem* fs) : fs(fs) {}

  ~AssetPool() { unloadAll(); }

  template <typename AssetType, typename... Args>
  void initializeAssetType(Args&&... args) {
    uint32_t asset_type = static_cast<uint32_t>(AssetType::ASSET_TYPE);
    const char* type_name = assets::EnumNameAssetType(AssetType::ASSET_TYPE);

    if (asset_type >= templates.size()) {
      templates.resize(asset_type + 1, nullptr);
    } else if (templates[asset_type]) {
      log_err("Attempted to initialize %s pool twice", type_name);
      return;
    }

    Asset* new_asset = new AssetType(args...);
    templates[asset_type] = new_asset;
  }

  template <typename AssetType>
  AssetHandle<AssetType> load(AssetId id) {
    uint32_t asset_type = static_cast<uint32_t>(AssetType::ASSET_TYPE);
    const char* type_name = assets::EnumNameAssetType(AssetType::ASSET_TYPE);

    {  // Check if this asset already exists
      auto iter = pool.find(id);

      if (iter != pool.end()) {
        AssetType* asset = dynamic_cast<AssetType*>(iter->second);

        if (asset == nullptr) {
          log_err("Cached asset 0x%0lx does not have type %s as expected", id,
                  type_name);
          return AssetHandle<AssetType>(nullptr);
        } else {
          return AssetHandle<AssetType>(id, asset);
        }
      }
    }

    if (templates.size() <= asset_type || templates[asset_type] == nullptr) {
      log_err("Attempted to load unitialized asset type %s", type_name);
      return AssetHandle<AssetType>(nullptr);
    }

    const assets::SerializedAsset* asset_data;
    bool loaded_successfully = fs->loadAsset(&asset_data, id);

    if (loaded_successfully) {
      if (asset_data->type() != AssetType::ASSET_TYPE) {
        loaded_successfully = false;
        log_err("SerializedAsset 0x%0lx does not have type %s as expected", id,
                type_name);
      }
    }

    if (!loaded_successfully) {
      log_err("Failed to load asset 0x%0lx", id);
      return AssetHandle<AssetType>(nullptr);
    }

    AssetType* template_asset = dynamic_cast<AssetType*>(templates[asset_type]);

    if (template_asset == nullptr) {
      log_err("Template asset for 0x%0lx does not have type %s as expected", id,
              type_name);
      return AssetHandle<AssetType>(nullptr);
    }

    AssetType* new_asset = new AssetType(*template_asset);
    new_asset->load(asset_data);
    new_asset->loaded = true;
    pool.emplace(id, new_asset);

    return AssetHandle<AssetType>(id, new_asset);
  }

  // TODO(marceline-cramer) Add garbage collecting method

  void unloadAll() {
    for (auto& asset : pool) {
      delete asset.second;
    }

    pool.clear();
  }

 private:
  Filesystem* fs;

  std::vector<Asset*> templates;
  std::unordered_map<AssetId, Asset*> pool;
};

}  // namespace mondradiko
