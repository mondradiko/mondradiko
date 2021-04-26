// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/scriptable/PointLightComponent.h"

#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

void PointLightComponent::getUniform(PointLightUniform* uniform) {
  auto position = glm::make_vec3(_data.position().v()->data());
  uniform->position = glm::vec4(position, 1.0);

  auto intensity = glm::make_vec3(_data.intensity().v()->data());
  uniform->intensity = glm::vec4(intensity, 1.0);
}

wasm_trap_t* PointLightComponent::setIntensity(ComponentScript*,
                                               const wasm_val_t args[],
                                               wasm_val_t[]) {
  auto intensity = glm::vec3(args[1].of.f64, args[2].of.f64, args[3].of.f64);
  protocol::GlmToVec3(&_data.mutable_intensity(), intensity);
  _updateAoe();
  return nullptr;
}

float PointLightComponent::_getMaxIntensity() {
  // "i" for intensity
  glm::vec3 i = glm::make_vec3(_data.intensity().v()->data());
  float max_i = i.r > i.g ? i.r : i.g;
  max_i = max_i > i.b ? max_i : i.b;
  return max_i;
}

void PointLightComponent::_updateAoe() { _aoe.radius = _getMaxIntensity(); }

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
