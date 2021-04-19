// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/MeshRendererComponent.h"

#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

void MeshRendererComponent::refresh(AssetPool* asset_pool) {
  mesh_asset = asset_pool->load<MeshAsset>(_data.mesh_asset());
  material_asset = asset_pool->load<MaterialAsset>(_data.material_asset());
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

}  // namespace core
}  // namespace mondradiko
