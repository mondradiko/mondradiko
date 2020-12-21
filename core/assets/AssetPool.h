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
  [[nodiscard]] AssetId loadAsset(AssetId id, Binding* binding) {
    if (isAssetLoaded<AssetType>(id)) return id;

    AssetId asset_entity = asset_registry.create(id);

    if (binding == nullptr) {
      DummyAsset asset_component;
      asset_component.type = typeid(AssetType).name();
      asset_component.binding = typeid(Binding).name();

      asset_registry.emplace<DummyAsset>(asset_entity, asset_component);
    } else {
      assets::ImmutableAsset asset_data;
      fs->loadAsset(&asset_data, id);

      AssetType* asset_component = new AssetType(asset_data, this, binding);
      asset_component->loaded = true;

      asset_registry.emplace<AssetType*>(asset_entity, asset_component);
    }

    return asset_entity;
  }

  template <typename AssetType>
  bool isAssetLoaded(AssetId id) const {
    if (!asset_registry.valid(id)) return false;

    if (asset_registry.has<DummyAsset>(id)) return false;

    AssetType* const* asset_ptr = asset_registry.try_get<AssetType*>(id);

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

  entt::basic_registry<AssetId> asset_registry;
};

}  // namespace mondradiko
