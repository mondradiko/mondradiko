// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/Component.h"
#include "lib/include/glm_headers.h"
#include "types/assets/PrefabAsset_generated.h"
#include "types/protocol/PointLightComponent_generated.h"

namespace mondradiko {
namespace core {

struct PointLightUniform {
  glm::vec4 position;
  glm::vec4 intensity;
};

class PointLightComponent : public Component<protocol::PointLightComponent> {
 public:
  explicit PointLightComponent(const protocol::PointLightComponent& data)
      : Component(data) {}

  explicit PointLightComponent(const assets::PointLightPrefab* prefab) {
    // TODO(marceline-cramer) Make helpers for these

    auto& position = _data.mutable_position();
    position.mutate_x(prefab->position().x());
    position.mutate_y(prefab->position().y());
    position.mutate_z(prefab->position().z());

    auto& intensity = _data.mutable_intensity();
    intensity.mutate_x(prefab->intensity().x());
    intensity.mutate_y(prefab->intensity().y());
    intensity.mutate_z(prefab->intensity().z());
  }

  PointLightComponent(double x, double y, double z, double r, double g,
                      double b) {
    _data.mutable_position().mutate_x(x);
    _data.mutable_position().mutate_y(y);
    _data.mutable_position().mutate_z(z);

    _data.mutable_intensity().mutate_x(r);
    _data.mutable_intensity().mutate_y(g);
    _data.mutable_intensity().mutate_z(b);
  }

  void getUniform(PointLightUniform*);

 private:
};

}  // namespace core
}  // namespace mondradiko
