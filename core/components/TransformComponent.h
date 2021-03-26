// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/Component.h"
#include "core/world/Entity.h"
#include "lib/include/glm_headers.h"
#include "lib/include/wasm_headers.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/TransformComponent_generated.h"

namespace mondradiko {

class TransformComponent : public Component<protocol::TransformComponent> {
 public:
  explicit TransformComponent(const protocol::TransformComponent& data)
      : Component(data) {}

  explicit TransformComponent(const assets::TransformPrefab* prefab) {
    // TODO(marceline-cramer) Make helpers for these

    auto& position = _data.mutable_position();
    position.mutate_x(prefab->position().x());
    position.mutate_y(prefab->position().y());
    position.mutate_z(prefab->position().z());

    auto& orientation = _data.mutable_orientation();
    orientation.mutate_w(prefab->orientation().w());
    orientation.mutate_x(prefab->orientation().x());
    orientation.mutate_y(prefab->orientation().y());
    orientation.mutate_z(prefab->orientation().z());
  }

  glm::mat4 getWorldTransform() const { return world_transform; }

  // Implement Component
  // Defined in generated API linker
  static void linkScriptApi(ScriptEnvironment*, World*);

 private:
  // Systems allowed to access private members directly
  friend class World;

  // System helpers
  // Used by World to calculate transforms
  glm::mat4 getLocalTransform();

  // Final transform result used in math
  glm::mat4 world_transform;

  //
  // Scripting methods
  //

  // Takes nothing, returns one f64
  wasm_trap_t* getX(const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getY(const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* getZ(const wasm_val_t[], wasm_val_t[]);
  // Takes three f64s, returns nothing
  wasm_trap_t* setPosition(const wasm_val_t[], wasm_val_t[]);
};

}  // namespace mondradiko
