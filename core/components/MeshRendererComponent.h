/**
 * @file MeshRendererComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all data required to render a mesh (geometry and material).
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/AssetPool.h"
#include "core/assets/MeshAsset.h"
#include "core/assets/MaterialAsset.h"

namespace mondradiko {

struct MeshRendererComponent {
  AssetId mesh_asset;
  AssetId material_asset;

  bool isLoaded(const AssetPool* asset_pool) const {
    return asset_pool->isAssetLoaded<MeshAsset>(mesh_asset) &&
           asset_pool->isAssetLoaded<MaterialAsset>(material_asset);
  }
};

}  // namespace mondradiko
