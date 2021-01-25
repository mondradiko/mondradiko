/**
 * @file TransformComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Transforms an entity and its children.
 * @date 2020-12-16
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/format/PrefabAsset_generated.h"
#include "core/common/glm_headers.h"
#include "core/common/wasm_headers.h"
#include "core/components/Component.h"
#include "core/world/Entity.h"
#include "protocol/TransformComponent_generated.h"

namespace mondradiko {

class TransformComponent : public Component<protocol::TransformComponent> {
 public:
  explicit TransformComponent(const protocol::TransformComponent& data)
      : Component(data) {}

  explicit TransformComponent(const assets::TransformPrefab* prefab) {
    _data.mutate_parent(static_cast<protocol::EntityId>(NullEntity));

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

  void setParent(EntityId parent) {
    _data.mutate_parent(static_cast<protocol::EntityId>(parent));
  }

  // Implement Component
  // Defined in generated API linker
  static void linkScriptApi(ScriptEnvironment*, World*);

 private:
  // Systems allowed to access private members directly
  friend class World;

  // System helpers
  // Used by World to calculate transforms
  EntityId getParent() const;
  glm::mat4 getLocalTransform();

  // Used to sort by parent
  EntityId this_entity;
  // Final transform result used in math
  glm::mat4 world_transform;

  //
  // Scripting methods
  //

  // Takes nothing, returns three f64s
  wasm_trap_t* getPosition(const wasm_val_t[], wasm_val_t[]);
  // Takes three f64s, returns nothing
  wasm_trap_t* setPosition(const wasm_val_t[], wasm_val_t[]);
};

}  // namespace mondradiko
