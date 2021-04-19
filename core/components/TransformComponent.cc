// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/TransformComponent.h"

#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

wasm_trap_t* TransformComponent::getX(ScriptEnvironment*,
                                      const wasm_val_t args[],
                                      wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.position().v()->Get(0);
  return nullptr;
}

wasm_trap_t* TransformComponent::getY(ScriptEnvironment*,
                                      const wasm_val_t args[],
                                      wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.position().v()->Get(1);
  return nullptr;
}

wasm_trap_t* TransformComponent::getZ(ScriptEnvironment*,
                                      const wasm_val_t args[],
                                      wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.position().v()->Get(2);
  return nullptr;
}

wasm_trap_t* TransformComponent::setPosition(ScriptEnvironment*,
                                             const wasm_val_t args[],
                                             wasm_val_t results[]) {
  auto position = glm::vec3(args[1].of.f64, args[2].of.f64, args[3].of.f64);
  protocol::GlmToVec3(&_data.mutable_position(), position);
  return nullptr;
}

wasm_trap_t* TransformComponent::getRotationW(ScriptEnvironment*,
                                              const wasm_val_t args[],
                                              wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.orientation().v()->Get(0);
  return nullptr;
}

wasm_trap_t* TransformComponent::getRotationX(ScriptEnvironment*,
                                              const wasm_val_t args[],
                                              wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.orientation().v()->Get(1);
  return nullptr;
}

wasm_trap_t* TransformComponent::getRotationY(ScriptEnvironment*,
                                              const wasm_val_t args[],
                                              wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.orientation().v()->Get(2);
  return nullptr;
}

wasm_trap_t* TransformComponent::getRotationZ(ScriptEnvironment*,
                                              const wasm_val_t args[],
                                              wasm_val_t results[]) {
  results[0].kind = WASM_F64;
  results[0].of.f64 = _data.orientation().v()->Get(3);
  return nullptr;
}

wasm_trap_t* TransformComponent::setRotation(ScriptEnvironment*,
                                             const wasm_val_t args[],
                                             wasm_val_t results[]) {
  auto orientation =
      glm::quat(args[1].of.f64, args[2].of.f64, args[3].of.f64, args[4].of.f64);
  protocol::GlmToQuat(&_data.mutable_orientation(), orientation);
  return nullptr;
}

glm::mat4 TransformComponent::getLocalTransform() {
  glm::quat orientation = glm::make_quat(_data.orientation().v()->data());
  glm::vec3 position = glm::make_vec3(_data.position().v()->data());

  glm::mat4 transform =
      glm::translate(glm::mat4(1.0), position) * glm::mat4(orientation);
  return transform;
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
