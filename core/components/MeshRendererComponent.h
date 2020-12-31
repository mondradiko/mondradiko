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

#include "core/assets/AssetPool.h"
#include "core/assets/MaterialAsset.h"
#include "core/assets/MeshAsset.h"
#include "core/components/Component.h"
#include "protocol/MeshRendererComponent_generated.h"

namespace mondradiko {

class MeshRendererComponent : public Component {
 public:
  explicit MeshRendererComponent(const protocol::MeshRendererComponent& _data)
      : data(_data) {}

  MeshRendererComponent(AssetId mesh_asset, AssetId material_asset)
      : data(static_cast<protocol::AssetId>(mesh_asset),
             static_cast<protocol::AssetId>(material_asset)) {}

  bool isLoaded(const AssetPool* asset_pool) const {
    return asset_pool->isAssetLoaded<MeshAsset>(getMeshAsset()) &&
           asset_pool->isAssetLoaded<MaterialAsset>(getMaterialAsset());
  }

  AssetId getMeshAsset() const {
    return static_cast<AssetId>(data.mesh_asset());
  }

  AssetId getMaterialAsset() const {
    return static_cast<AssetId>(data.material_asset());
  }

  // Serialization methods
  using SerializedType = protocol::MeshRendererComponent;
  const SerializedType& getData() { return data; }
  void writeData(const SerializedType& _data) { data = _data; }

 private:
  SerializedType data;
};

}  // namespace mondradiko
