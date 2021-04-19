// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/Component.h"
#include "core/scripting/ScriptObject.h"
#include "core/world/Entity.h"
#include "lib/include/glm_headers.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/ProtocolTypes.h"
#include "types/protocol/TransformComponent_generated.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ScriptEnvironment;

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

  glm::mat4 getWorldTransform() const { return world_transform; }

  //
  // Scripting methods
  //

  // Takes nothing, returns one f64
  wasm_trap_t* getX(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getY(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getZ(ScriptEnvironment*, const wasm_val_t[], wasm_val_t[]);
  // Takes three f64s, returns nothing
  wasm_trap_t* setPosition(ScriptEnvironment*, const wasm_val_t[],
                           wasm_val_t[]);

  // Takes nothing, returns one f64
  wasm_trap_t* getRotationW(ScriptEnvironment*, const wasm_val_t[],
                            wasm_val_t[]);
  wasm_trap_t* getRotationX(ScriptEnvironment*, const wasm_val_t[],
                            wasm_val_t[]);
  wasm_trap_t* getRotationY(ScriptEnvironment*, const wasm_val_t[],
                            wasm_val_t[]);
  wasm_trap_t* getRotationZ(ScriptEnvironment*, const wasm_val_t[],
                            wasm_val_t[]);

  // Takes four f64s, returns nothing
  wasm_trap_t* setRotation(ScriptEnvironment*, const wasm_val_t[],
                           wasm_val_t[]);

 private:
  // Systems allowed to access private members directly
  friend class World;

  // System helpers
  // Used by World to calculate transforms
  glm::mat4 getLocalTransform();

  // Final transform result used in math
  glm::mat4 world_transform;
};

}  // namespace core
}  // namespace mondradiko
