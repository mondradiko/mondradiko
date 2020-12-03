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

#ifndef SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_
#define SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_

#include "assets/AssetHandle.h"
#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "src/api_headers.h"

namespace mondradiko {

class MeshRendererComponent {
 public:
  AssetHandle<MeshAsset> mesh_asset;
  AssetHandle<MaterialAsset> material_asset;
};

}  // namespace mondradiko

#endif  // SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_
