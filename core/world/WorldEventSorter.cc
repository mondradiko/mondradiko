/**
 * @file WorldEventSorter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Determines which WorldEvents go to which clients.
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/world/WorldEventSorter.h"

#include <utility>
#include <vector>

#include "core/components/TransformComponent.h"
#include "core/world/World.h"
#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

WorldEventSorter::WorldEventSorter(World* world) : world(world) {}

WorldEventSorter::~WorldEventSorter() {}

void WorldEventSorter::processEvent(
    std::unique_ptr<protocol::WorldEventT>& event) {
  global_events.push_back(std::move(event));
}

// Helper function to generate component update events
template <class ComponentType, class ProtocolComponentType>
flatbuffers::Offset<protocol::WorldEvent> updateComponents(
    flatbuffers::FlatBufferBuilder& builder, EntityRegistry& registry) {
  // TODO(marceline-cramer) static_assert ComponentType for methods

  auto component_view = registry.view<ComponentType>();
  std::vector<EntityId> entities_data(component_view.size());
  std::vector<ProtocolComponentType> components_data(component_view.size());

  uint32_t component_index = 0;

  for (auto& entity : component_view) {
    entities_data[component_index] = entity;
    // TODO(marceline-cramer) Write component data directly to flatbuffer
    components_data[component_index] = component_view.get(entity).getData();
    component_index++;
  }

  auto entities_offset = builder.CreateVector(entities_data);
  auto components_offset = builder.CreateVectorOfStructs(components_data);

  protocol::UpdateComponentsBuilder update_components(builder);
  update_components.add_entities(entities_offset);
  buildUpdateComponents(update_components, components_offset);
  auto update_components_offset = update_components.Finish();

  protocol::WorldEventBuilder world_event(builder);
  world_event.add_type(protocol::WorldEventType::UpdateComponents);
  world_event.add_update_components(update_components_offset);
  auto world_event_offset = world_event.Finish();

  return world_event_offset;
}

WorldEventSorter::WorldUpdateOffset WorldEventSorter::broadcastGlobalEvents(
    flatbuffers::FlatBufferBuilder& builder) const {
  std::vector<flatbuffers::Offset<protocol::WorldEvent>> update_offsets;

  for (auto& event : global_events) {
    auto event_offset = protocol::CreateWorldEvent(builder, event.get());
    update_offsets.push_back(event_offset);
  }

  update_offsets.push_back(
      updateComponents<TransformComponent, protocol::TransformComponent>(
          builder, world->registry));

  return builder.CreateVector(update_offsets);
}

bool WorldEventSorter::isOutOfDate() { return global_events.size() > 0; }

void WorldEventSorter::clearQueue() { global_events.clear(); }

}  // namespace mondradiko
