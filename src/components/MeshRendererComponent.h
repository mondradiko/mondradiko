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
#include "components/Component.h"
#include "src/api_headers.h"

namespace mondradiko {

class MeshPipeline;  // Forward declaration because of codependence

class MeshRendererComponent : public Component {
 public:
  MeshRendererComponent(MeshPipeline*, AssetHandle<MeshAsset>&,
                        AssetHandle<MaterialAsset>&);
  ~MeshRendererComponent();

  AssetHandle<MeshAsset> mesh_asset;
  AssetHandle<MaterialAsset> material_asset;

 private:
  MeshPipeline* mesh_pipeline;
};

}  // namespace mondradiko

#endif  // SRC_COMPONENTS_MESHRENDERERCOMPONENT_H_
