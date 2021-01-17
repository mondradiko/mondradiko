/**
 * @file MeshRendererComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Renders a mesh.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/format/PrefabAsset_generated.h"
#include "core/assets/MaterialAsset.h"
#include "core/assets/MeshAsset.h"
#include "core/components/Component.h"
#include "protocol/MeshRendererComponent_generated.h"

namespace mondradiko {

class MeshRendererComponent
    : public Component<protocol::MeshRendererComponent> {
 public:
  explicit MeshRendererComponent(const protocol::MeshRendererComponent& data)
      : Component(data) {}

  explicit MeshRendererComponent(const assets::MeshRendererPrefabT* prefab) {
    _data.mutate_mesh_asset(static_cast<protocol::AssetId>(prefab->mesh));
    _data.mutate_material_asset(
        static_cast<protocol::AssetId>(prefab->material));
  }

  MeshRendererComponent(AssetId mesh_asset, AssetId material_asset)
      : Component(protocol::MeshRendererComponent(
            static_cast<protocol::AssetId>(mesh_asset),
            static_cast<protocol::AssetId>(material_asset))) {}

  bool isLoaded() const { return getMeshAsset() && getMaterialAsset(); }

  const AssetHandle<MeshAsset>& getMeshAsset() const { return mesh_asset; }

  const AssetHandle<MaterialAsset>& getMaterialAsset() const {
    return material_asset;
  }

 private:
  AssetHandle<MeshAsset> mesh_asset;
  AssetHandle<MaterialAsset> material_asset;
};

}  // namespace mondradiko
