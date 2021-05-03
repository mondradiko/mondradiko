// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/InternalComponent.h"
#include "core/world/Entity.h"

namespace mondradiko {
namespace core {

class ChildComponent : public InternalComponent {
 public:
  ChildComponent(EntityId self) : _prev_sibling(self), _next_sibling(self) {}

 private:
  friend class World;

  EntityId _parent = NullEntity;
  EntityId _prev_sibling;
  EntityId _next_sibling;
};

}  // namespace core
}  // namespace mondradiko
