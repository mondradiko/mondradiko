// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/InternalComponent.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

class PointerComponent : public InternalComponent {
 public:
  PointerComponent(const glm::vec3& position, const glm::vec3& direction)
      : _position(position), _direction(direction) {}

  const glm::vec3& getPosition() const { return _position; }
  const glm::vec3& getDirection() const { return _direction; }

  void setSelect() { _select = true; }

  bool checkSelect() {
    if (_select) {
      _select = false;
      return true;
    }

    return false;
  }

 private:
  glm::vec3 _position;
  glm::vec3 _direction;

  bool _select = false;
};

}  // namespace core
}  // namespace mondradiko
