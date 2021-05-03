// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/InternalComponent.h"
#include "core/world/Entity.h"

namespace mondradiko {
namespace core {

class ParentComponent : public InternalComponent {
 public:
  ParentComponent() : _child_num(0), _first_child(NullEntity) {}
  ParentComponent(EntityId child) : _child_num(1), _first_child(child) {}

 private:
  friend class World;

  size_t _child_num;
  EntityId _first_child;
};

}  // namespace core
}  // namespace mondradiko
