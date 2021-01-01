/**
 * @file ScriptComponent.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Binds a ScriptAsset and its events to an entity.
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/components/ScriptComponent.h"

#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

// Template specialization to build UpdateComponents event
template <>
void buildUpdateComponents<protocol::ScriptComponent>(
    protocol::UpdateComponentsBuilder* update_components,
    flatbuffers::Offset<flatbuffers::Vector<const protocol::ScriptComponent*>>
        components) {
  update_components->add_type(protocol::ComponentType::ScriptComponent);
  update_components->add_script(components);
}

}  // namespace mondradiko
