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

  MeshRendererComponent(AssetId mesh_asset, AssetId material_asset)
      : Component(protocol::MeshRendererComponent(
            static_cast<protocol::AssetId>(mesh_asset),
            static_cast<protocol::AssetId>(material_asset))) {}

  bool isLoaded(AssetPool* asset_pool) const {
    MaterialAsset& material =
        asset_pool->getAsset<MaterialAsset>(getMaterialAsset());
    return asset_pool->getAsset<MeshAsset>(getMeshAsset()).isLoaded() &&
           material.isLoaded() && material.isTextureLoaded(asset_pool);
  }

  AssetId getMeshAsset() const {
    return static_cast<AssetId>(_data.mesh_asset());
  }

  AssetId getMaterialAsset() const {
    return static_cast<AssetId>(_data.material_asset());
  }
};

}  // namespace mondradiko
