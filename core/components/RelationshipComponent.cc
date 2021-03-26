// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/RelationshipComponent.h"

#include "core/world/World.h"
#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {

RelationshipComponent::RelationshipComponent(EntityId self_id)
    : _self_id(self_id) {
  _data.mutate_child_num(0);
  _data.mutate_first_child(static_cast<protocol::EntityId>(NullEntity));
  _data.mutate_parent(static_cast<protocol::EntityId>(NullEntity));
  _data.mutate_prev_child(static_cast<protocol::EntityId>(self_id));
  _data.mutate_next_child(static_cast<protocol::EntityId>(self_id));
}

bool RelationshipComponent::_hasParent() {
  return static_cast<EntityId>(_data.parent()) != NullEntity;
}

void RelationshipComponent::_adopt(RelationshipComponent* other, World* world) {
  // Set ourselves as the new parent
  other->_orphan(world);
  other->_data.mutate_parent(static_cast<protocol::EntityId>(_self_id));

  auto child_id = static_cast<protocol::EntityId>(other->_self_id);

  // If we have other children already, link them
  if (_data.child_num() != 0) {
    // Get our first child
    auto first_id = static_cast<EntityId>(_data.first_child());
    auto& first_child = world->registry.get<RelationshipComponent>(first_id);

    // Get our second child
    auto next_id = static_cast<EntityId>(first_child._data.next_child());
    auto& next_child = world->registry.get<RelationshipComponent>(next_id);

    // Link the new child to its new siblings
    other->_data.mutate_prev_child(next_child._data.prev_child());
    other->_data.mutate_next_child(first_child._data.next_child());

    // Link siblings
    first_child._data.mutate_next_child(child_id);
    next_child._data.mutate_prev_child(child_id);
  } else {
    // Otherwise, set this as our first and only child
    _data.mutate_first_child(child_id);

    // The first child loops back to itself
    other->_data.mutate_prev_child(child_id);
    other->_data.mutate_next_child(child_id);
  }

  _data.mutate_child_num(_data.child_num() + 1);
}

void RelationshipComponent::_orphan(World* world) {
  if (!_hasParent()) return;

  auto parent_id = static_cast<EntityId>(_data.parent());
  auto& parent = world->registry.get<RelationshipComponent>(parent_id);

  if (_self_id == static_cast<EntityId>(parent._data.first_child())) {
    parent._data.mutate_first_child(_data.next_child());
  }

  auto prev_id = static_cast<EntityId>(_data.prev_child());
  auto& prev = world->registry.get<RelationshipComponent>(prev_id);

  auto next_id = static_cast<EntityId>(_data.next_child());
  auto& next = world->registry.get<RelationshipComponent>(next_id);

  prev._data.mutate_next_child(_data.next_child());
  next._data.mutate_prev_child(_data.prev_child());

  parent._data.mutate_child_num(parent._data.child_num() - 1);
  _data.mutate_parent(static_cast<protocol::EntityId>(NullEntity));
}

// Template specialization to build UpdateComponents event
template <>
void buildUpdateComponents<protocol::RelationshipComponent>(
    protocol::UpdateComponentsBuilder* update_components,
    flatbuffers::Offset<
        flatbuffers::Vector<const protocol::RelationshipComponent*>>
        components) {
  update_components->add_type(protocol::ComponentType::RelationshipComponent);
  update_components->add_relationship(components);
}

}  // namespace mondradiko
