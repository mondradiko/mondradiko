// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/ScriptableComponent.h"
#include "lib/include/glm_headers.h"
#include "lib/include/wasm_headers.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/PointLightComponent_generated.h"
#include "types/protocol/ProtocolTypes.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ComponentScript;

struct PointLightUniform {
  glm::vec4 position;
  glm::vec4 intensity;
};

class PointLightComponent
    : public ScriptableComponent<PointLightComponent,
                                 protocol::PointLightComponent> {
 public:
  explicit PointLightComponent(const protocol::PointLightComponent& data)
      : ScriptableComponent(data) {}

  explicit PointLightComponent(const assets::PointLightPrefab* prefab) {
    auto position = glm::make_vec3(prefab->position().v()->data());
    protocol::GlmToVec3(&_data.mutable_position(), position);

    auto intensity = glm::make_vec3(prefab->intensity().v()->data());
    protocol::GlmToVec3(&_data.mutable_intensity(), intensity);
  }

  PointLightComponent(const glm::vec3& position, const glm::vec3& intensity) {
    protocol::GlmToVec3(&_data.mutable_position(), position);
    protocol::GlmToVec3(&_data.mutable_intensity(), intensity);
  }

  PointLightComponent()
      : PointLightComponent(glm::vec3(0.0, 0.0, 0.0),
                            glm::vec3(1.0, 0.0, 0.0)) {}

  void getUniform(PointLightUniform*);

  //
  // Script methods
  //
  wasm_trap_t* setIntensity(ComponentScript*, const wasm_val_t[], wasm_val_t[]);

 private:
};

}  // namespace core
}  // namespace mondradiko
