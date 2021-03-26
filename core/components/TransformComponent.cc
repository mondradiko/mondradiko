// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/TransformComponent.h"

#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

EntityId TransformComponent::getParent() const {
  return static_cast<EntityId>(_data.parent());
}

glm::mat4 TransformComponent::getLocalTransform() {
  auto orientation = _data.orientation();
  auto position = _data.position();

  glm::quat converted_orientation(orientation.w(), orientation.x(),
                                  orientation.y(), orientation.z());
  glm::vec3 converted_position(position.x(), position.y(), position.z());

  glm::mat4 transform =
      glm::translate(glm::mat4(converted_orientation), converted_position);
  return transform;
}

wasm_trap_t* TransformComponent::getX(const wasm_val_t args[],
                                      wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.position().x();
  return nullptr;
}

wasm_trap_t* TransformComponent::getY(const wasm_val_t args[],
                                      wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.position().y();
  return nullptr;
}

wasm_trap_t* TransformComponent::getZ(const wasm_val_t args[],
                                      wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.position().z();
  return nullptr;
}

wasm_trap_t* TransformComponent::setPosition(const wasm_val_t args[],
                                             wasm_val_t results[]) {
  _data.mutable_position().mutate_x(args[1].of.f64);
  _data.mutable_position().mutate_y(args[2].of.f64);
  _data.mutable_position().mutate_z(args[3].of.f64);
  return nullptr;
}

// Template specialization to build UpdateComponents event
template <>
void buildUpdateComponents<protocol::TransformComponent>(
    protocol::UpdateComponentsBuilder* update_components,
    flatbuffers::Offset<
        flatbuffers::Vector<const protocol::TransformComponent*>>
        components) {
  update_components->add_type(protocol::ComponentType::TransformComponent);
  update_components->add_transform(components);
}

}  // namespace core
}  // namespace mondradiko
