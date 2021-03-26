// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/world/World.h"

#include <deque>
#include <iostream>
#include <vector>

#include "core/assets/PrefabAsset.h"
#include "core/components/MeshRendererComponent.h"
#include "core/components/PointLightComponent.h"
#include "core/components/RelationshipComponent.h"
#include "core/components/ScriptComponent.h"
#include "core/components/TransformComponent.h"
#include "core/filesystem/Filesystem.h"
#include "core/scripting/ScriptEnvironment.h"
#include "log/log.h"
#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {

World::World(AssetPool* asset_pool, Filesystem* fs, ScriptEnvironment* scripts)
    : asset_pool(asset_pool), fs(fs), scripts(scripts) {
  log_zone;

  asset_pool->initializeAssetType<PrefabAsset>(asset_pool);

  scripts->initializeAssets(asset_pool);
  scripts->linkComponentApis(this);
}

World::~World() { log_zone; }

void World::initializePrefabs() {
  std::vector<AssetId> prefabs;
  fs->getInitialPrefabs(prefabs);

  for (auto prefab_id : prefabs) {
    auto prefab = asset_pool->load<PrefabAsset>(prefab_id);
    prefab->instantiate(scripts, this);
  }
}

///////////////////////////////////////////////////////////////////////////////
// World event callbacks
///////////////////////////////////////////////////////////////////////////////

void World::onSpawnEntity(const protocol::SpawnEntity* event) {
  EntityId id = static_cast<EntityId>(event->new_id());

  if (registry.valid(id)) {
    log_wrn_fmt("Spawned entity's ID is taken: 0x%0dx", id);
    return;
  }

  // Discard returned ID, since we just ensured that the hint will be used
  static_cast<void>(registry.create(id));
}

void World::onUpdateComponents(
    const protocol::UpdateComponents* update_components) {
  auto entities = update_components->entities();

  switch (update_components->type()) {
    case protocol::ComponentType::NoComponent: {
      log_dbg("Received empty component update");
      break;
    }

    case protocol::ComponentType::MeshRendererComponent: {
      updateComponents<MeshRendererComponent>(
          entities, update_components->mesh_renderer());
      break;
    }

    case protocol::ComponentType::PointLightComponent: {
      updateComponents<PointLightComponent>(entities,
                                            update_components->point_light());
      break;
    }

    case protocol::ComponentType::RelationshipComponent: {
      updateComponents<RelationshipComponent>(
          entities, update_components->relationship());
      break;
    }

    case protocol::ComponentType::TransformComponent: {
      updateComponents<TransformComponent>(entities,
                                           update_components->transform());
      break;
    }

    default: {
      log_err_fmt("Unrecognized component type %hu", update_components->type());
      break;
    }
  }
}

void World::onUpdateScripts(const protocol::UpdateScripts* update_scripts) {}

///////////////////////////////////////////////////////////////////////////////
// Helper methods
///////////////////////////////////////////////////////////////////////////////

bool World::update(double dt) {
  log_zone;

  {
    log_zone_named("Process free transforms");

    auto transforms = registry.group<TransformComponent>(
        entt::exclude<RelationshipComponent>);

    for (auto e : transforms) {
      auto& transform = transforms.get<TransformComponent>(e);
      transform.world_transform = transform.getLocalTransform();
    }
  }

  {
    log_zone_named("Process transform hierarchy");

    // This is a rather naive approach to sorting the transform hierarchy.
    // Optimally, it'd be tracking which transforms/relationships have been
    // changed with a dirty component, but doing an exhaustive search of the
    // hierarchy will do the job for now. If this zone starts taking up too
    // much time in the future, then we need to optimize it.

    // (child, parent)
    std::deque<std::pair<EntityId, EntityId>> process_queue;

    {  // Find all top-level entities (without parents)
      auto relationship_view = registry.view<RelationshipComponent>();

      for (auto e : relationship_view) {
        auto& rel = relationship_view.get(e);

        if (!rel._hasParent()) {
          process_queue.emplace_front(std::make_pair(e, NullEntity));
        }
      }
    }

    // Propogate world transforms through hierarchy
    while (!process_queue.empty()) {
      // Get next entity
      auto process_id = process_queue.back();
      process_queue.pop_back();
      EntityId self_id = process_id.first;
      EntityId parent_id = process_id.second;

      // Calculate transform
      if (registry.has<TransformComponent>(self_id)) {
        glm::mat4 parent_transform = glm::mat4(1.0);
        if (parent_id != NullEntity) {
          auto& transform = registry.get<TransformComponent>(parent_id);
          parent_transform = transform.getWorldTransform();
        }

        auto& self_transform = registry.get<TransformComponent>(self_id);
        glm::mat4 local_transform = self_transform.getLocalTransform();
        self_transform.world_transform = parent_transform * local_transform;

        // Use ourselves as the transform parent for our children
        parent_id = self_id;
      }

      // Add children to queue
      auto& self_rel = registry.get<RelationshipComponent>(self_id);
      if (self_rel._data.child_num() > 0) {
        auto first_child = static_cast<EntityId>(self_rel._data.first_child());
        auto current_child = first_child;
        do {
          auto& next_child = registry.get<RelationshipComponent>(current_child);
          process_queue.emplace_front(std::make_pair(current_child, parent_id));
          current_child = static_cast<EntityId>(next_child._data.next_child());
        } while (first_child != current_child);
      }
    }
  }

  scripts->update(&registry, asset_pool, dt);

  log_frame_mark;
  return true;
}

void World::processEvent(const protocol::WorldEvent* event) {
  switch (event->type()) {
    case protocol::WorldEventType::NoEvent: {
      log_dbg("Received empty world event");
      break;
    }

    case protocol::WorldEventType::SpawnEntity: {
      onSpawnEntity(event->spawn_entity());
      break;
    }

    case protocol::WorldEventType::UpdateComponents: {
      onUpdateComponents(event->update_components());
      break;
    }

    case protocol::WorldEventType::UpdateScripts: {
      onUpdateScripts(event->update_scripts());
      break;
    }

    default: {
      log_err_fmt("Unrecognized world event %hu", event->type());
      break;
    }
  }
}

template <class ComponentType, class ProtocolComponentType>
void World::updateComponents(
    const flatbuffers::Vector<EntityId>* entities,
    const flatbuffers::Vector<const ProtocolComponentType*>* components) {
  if (entities->size() != components->size()) {
    log_err("Size mismatch between entities and components");
    return;
  }

  for (uint32_t i = 0; i < entities->size(); i++) {
    EntityId id = entities->Get(i);

    if (!registry.valid(id)) {
      // Discard returned ID, since we just ensured that the hint will be used
      static_cast<void>(registry.create(id));
    }

    const ProtocolComponentType& component = *components->Get(i);

    if (registry.has<ComponentType>(id)) {
      ComponentType& handle = registry.get<ComponentType>(id);
      handle.writeData(component);
      handle.refresh(asset_pool);
    } else {
      ComponentType& handle = registry.emplace<ComponentType>(id, component);
      handle.refresh(asset_pool);
    }
  }
}

}  // namespace mondradiko
