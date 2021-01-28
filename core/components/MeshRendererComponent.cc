// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/MeshRendererComponent.h"

#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

void MeshRendererComponent::refresh(AssetPool* asset_pool) {
  auto mesh_id = static_cast<AssetId>(_data.mesh_asset());
  auto material_id = static_cast<AssetId>(_data.material_asset());

  mesh_asset = asset_pool->load<MeshAsset>(mesh_id);
  material_asset = asset_pool->load<MaterialAsset>(material_id);
}

// Template specialization to build UpdateComponents event
template <>
void buildUpdateComponents<protocol::MeshRendererComponent>(
    protocol::UpdateComponentsBuilder* update_components,
    flatbuffers::Offset<
        flatbuffers::Vector<const protocol::MeshRendererComponent*>>
        components) {
  update_components->add_type(protocol::ComponentType::MeshRendererComponent);
  update_components->add_mesh_renderer(components);
}

}  // namespace mondradiko
