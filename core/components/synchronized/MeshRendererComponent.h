// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/MaterialAsset.h"
#include "core/assets/MeshAsset.h"
#include "core/components/SynchronizedComponent.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/MeshRendererComponent_generated.h"

namespace mondradiko {
namespace core {

class MeshRendererComponent
    : public SynchronizedComponent<protocol::MeshRendererComponent> {
 public:
  explicit MeshRendererComponent(const protocol::MeshRendererComponent& data)
      : SynchronizedComponent(data) {}

  explicit MeshRendererComponent(const assets::MeshRendererPrefab* prefab) {
    _data.mutate_mesh_asset(prefab->mesh());
    _data.mutate_material_asset(prefab->material());
  }

  MeshRendererComponent(AssetId mesh_asset, AssetId material_asset)
      : SynchronizedComponent(
            protocol::MeshRendererComponent(mesh_asset, material_asset)) {}

  // Component implementation
  void refresh(AssetPool*);

  bool isLoaded() const { return getMeshAsset() && getMaterialAsset(); }

  const AssetHandle<MeshAsset>& getMeshAsset() const { return mesh_asset; }

  const AssetHandle<MaterialAsset>& getMaterialAsset() const {
    return material_asset;
  }

 private:
  AssetHandle<MeshAsset> mesh_asset;
  AssetHandle<MaterialAsset> material_asset;
};

}  // namespace core
}  // namespace mondradiko
