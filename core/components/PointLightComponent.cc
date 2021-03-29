// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/PointLightComponent.h"

#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

void PointLightComponent::getUniform(PointLightUniform* uniform) {
  // TODO(marceline-cramer) Make helpers for these
  uniform->position.x = _data.position().x();
  uniform->position.y = _data.position().y();
  uniform->position.z = _data.position().z();
  uniform->position.w = 1.0;

  uniform->intensity.r = _data.intensity().x();
  uniform->intensity.g = _data.intensity().y();
  uniform->intensity.b = _data.intensity().z();
}

// Template specialization to build UpdateComponents event
template <>
void buildUpdateComponents<mondradiko::protocol::PointLightComponent>(
    protocol::UpdateComponentsBuilder* update_components,
    flatbuffers::Offset<
        flatbuffers::Vector<const mondradiko::protocol::PointLightComponent*>>
        components) {
  update_components->add_type(protocol::ComponentType::PointLightComponent);
  update_components->add_point_light(components);
}

}  // namespace core
}  // namespace mondradiko
