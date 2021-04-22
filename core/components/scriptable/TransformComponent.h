// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/ScriptableComponent.h"
#include "core/world/Entity.h"
#include "lib/include/glm_headers.h"
#include "lib/include/wasm_headers.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/ProtocolTypes.h"
#include "types/protocol/TransformComponent_generated.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ComponentScript;

class TransformComponent
    : public ScriptableComponent<TransformComponent,
                                 protocol::TransformComponent> {
 public:
  explicit TransformComponent(const protocol::TransformComponent& data)
      : ScriptableComponent(data) {}

  explicit TransformComponent(const assets::TransformPrefab* prefab) {
    auto position = glm::make_vec3(prefab->position().v()->data());
    protocol::GlmToVec3(&_data.mutable_position(), position);

    auto orientation = glm::make_quat(prefab->orientation().v()->data());
    protocol::GlmToQuat(&_data.mutable_orientation(), orientation);
  }

  TransformComponent(const glm::vec3& position, const glm::quat& orientation) {
    protocol::GlmToVec3(&_data.mutable_position(), position);
    protocol::GlmToQuat(&_data.mutable_orientation(), orientation);
  }

  TransformComponent()
      : TransformComponent(glm::vec3(0.0, 0.0, 0.0), glm::quat()) {}

  //
  // Scripting methods
  //

  // Takes nothing, returns one f64
  wasm_trap_t* getX(ComponentScript*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getY(ComponentScript*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getZ(ComponentScript*, const wasm_val_t[], wasm_val_t[]);
  // Takes three f64s, returns nothing
  wasm_trap_t* setPosition(ComponentScript*, const wasm_val_t[], wasm_val_t[]);

  // Takes nothing, returns one f64
  wasm_trap_t* getRotationW(ComponentScript*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getRotationX(ComponentScript*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getRotationY(ComponentScript*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getRotationZ(ComponentScript*, const wasm_val_t[], wasm_val_t[]);

  // Takes four f64s, returns nothing
  wasm_trap_t* setRotation(ComponentScript*, const wasm_val_t[], wasm_val_t[]);

 private:
  // Systems allowed to access private members directly
  friend class World;

  // System helpers
  // Used by World to calculate transforms
  glm::mat4 getLocalTransform();
};

}  // namespace core
}  // namespace mondradiko
