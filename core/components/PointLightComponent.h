/**
 * @file PointLightComponent.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Illuminates a world with an omnidirectional point light.
 * @date 2021-01-26
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 *
 */

#pragma once

#include "assets/format/PrefabAsset_generated.h"
#include "core/common/glm_headers.h"
#include "core/components/Component.h"
#include "protocol/PointLightComponent_generated.h"

namespace mondradiko {

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

}  // namespace mondradiko
