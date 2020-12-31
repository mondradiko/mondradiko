/**
 * @file MeshRendererComponent.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Renders a mesh.
 * @date 2020-12-30
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/components/MeshRendererComponent.h"

#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

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
