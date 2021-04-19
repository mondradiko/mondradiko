// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/components/SynchronizedComponent.h"
#include "core/world/Entity.h"
#include "types/protocol/RelationshipComponent_generated.h"

namespace mondradiko {
namespace core {

class RelationshipComponent
    : public SynchronizedComponent<protocol::RelationshipComponent> {
 public:
  explicit RelationshipComponent(const protocol::RelationshipComponent& data)
      : SynchronizedComponent(data) {}

  explicit RelationshipComponent(EntityId);

 private:
  // Systems allowed to access private members directly
  friend class World;

  // Helper methods
  bool _hasParent();
  void _adopt(RelationshipComponent*, World*);
  void _orphan(World*);

  EntityId _self_id;
};

}  // namespace core
}  // namespace mondradiko
