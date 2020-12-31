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

#include "assets/loading/ImmutableAsset.h"
#include "core/assets/Asset.h"
#include "core/common/api_headers.h"
#include "core/filesystem/Filesystem.h"
#include "log/log.h"

namespace mondradiko {

struct DummyAsset {
  std::string type;
  std::string binding;
};

class AssetPool {
 public:
  explicit AssetPool(Filesystem* fs) : fs(fs) {}

  template <typename AssetType, typename Binding>
  AssetId loadAsset(AssetId id, Binding* binding) {
    if (isAssetLoaded<AssetType>(id)) return id;

    AssetId local_id = asset_registry.create(id);
    local_ids.emplace(id, local_id);

    if (binding == nullptr) {
      DummyAsset asset_component;
      asset_component.type = typeid(AssetType).name();
      asset_component.binding = typeid(Binding).name();

      asset_registry.emplace<DummyAsset>(local_id, asset_component);
    } else {
      assets::ImmutableAsset asset_data;

      if (!fs->loadAsset(&asset_data, id)) {
        DummyAsset asset_component;
        asset_component.type = typeid(AssetType).name();
        asset_component.binding = typeid(Binding).name();

        asset_registry.emplace<DummyAsset>(local_id, asset_component);
        return id;
      }

      AssetType* asset_component = new AssetType(asset_data, this, binding);
      asset_component->loaded = true;

      asset_registry.emplace<AssetType*>(local_id, asset_component);
    }

    return id;
  }

  template <typename AssetType>
  bool isAssetLoaded(AssetId id) const {
    auto iter = local_ids.find(id);
    if (iter == local_ids.end()) return false;
    AssetId local_id = iter->second;

    if (!asset_registry.valid(local_id)) return false;

    if (asset_registry.has<DummyAsset>(local_id)) return false;

    AssetType* const* asset_ptr = asset_registry.try_get<AssetType*>(local_id);

    if (!asset_ptr) return false;

    AssetType* asset = *asset_ptr;

    if (asset) {
      return asset->loaded;
    } else {
      return false;
    }
  }

  template <typename AssetType>
  const AssetType* getAsset(AssetId id) const {
    // TODO(marceline-cramer) Load here if asset is invalid
    if (!isAssetLoaded<AssetType>(id)) {
      log_ftl("Tried to get asset 0x%0lx of type %s, which was invalid.", id,
              typeid(AssetType).name());
    }

    return asset_registry.get<AssetType*>(id);
  }

  template <typename AssetType>
  void unloadAll() {
    auto assets_view = asset_registry.view<AssetType*>();
    for (auto asset : assets_view) {
      delete assets_view.get(asset);
    }
    asset_registry.destroy(assets_view.begin(), assets_view.end());
  }

 private:
  Filesystem* fs;

  std::unordered_map<AssetId, AssetId> local_ids;
  entt::basic_registry<AssetId> asset_registry;
};

}  // namespace mondradiko
