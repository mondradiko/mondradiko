// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/InternalComponent.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

class WorldTransform : public InternalComponent {
 public:
  WorldTransform(const glm::mat4& transform) : _transform(transform) {}

  WorldTransform(const glm::vec3& position, const glm::quat& orientation) {
    auto translation = glm::translate(glm::mat4(1.0), position);
    auto rotation = glm::mat4(orientation);
    _transform = translation * rotation;
  }

  const glm::mat4& getTransform() { return _transform; }

 private:
  glm::mat4 _transform;
};

}  // namespace core
}  // namespace mondradiko
