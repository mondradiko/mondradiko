/**
 * @file MeshRendererComponent.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all data required to render a mesh (geometry and material).
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "components/MeshRendererComponent.h"

#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "renderer/MeshPipeline.h"

namespace mondradiko {

MeshRendererComponent::MeshRendererComponent(
    MeshPipeline* mesh_pipeline, AssetHandle<MeshAsset>& mesh_asset,
    AssetHandle<MaterialAsset>& material_asset)
    : mesh_asset(mesh_asset),
      material_asset(material_asset),
      mesh_pipeline(mesh_pipeline) {
  mesh_pipeline->mesh_renderers.insert(this);
}

MeshRendererComponent::~MeshRendererComponent() {
  mesh_pipeline->mesh_renderers.erase(this);
}

}  // namespace mondradiko
