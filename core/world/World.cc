// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/world/World.h"

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
    prefab->instantiate(&registry, scripts);
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
    log_zone_named("Update TransformComponent self-reference");

    auto transform_view = registry.view<TransformComponent>();

    for (auto e : transform_view) {
      TransformComponent& transform = transform_view.get(e);
      transform.this_entity = e;
    }
  }

  {
    log_zone_named("Sort transform hierarchy");

    registry.sort<TransformComponent>(
        [](const TransformComponent& lhs, const TransformComponent& rhs) {
          // Sort children after parents
          if (lhs.this_entity == rhs.getParent()) {
            return true;
          }
          return false;
        });
  }

  {
    log_zone_named("Transform children under parents");

    auto transform_view = registry.view<TransformComponent>();

    for (auto e : transform_view) {
      TransformComponent& transform = transform_view.get(e);

      auto parent = transform.getParent();
      glm::mat4 parent_transform;
      if (parent == NullEntity) {
        parent_transform = glm::mat4(1.0);
      } else {
        if (!registry.valid(parent)) {
          log_err("Invalid Transform parent ID");
          continue;
        }

        if (!registry.has<TransformComponent>(parent)) {
          log_err("Transform parent has no Transform");
          continue;
        }

        parent_transform =
            registry.get<TransformComponent>(parent).getWorldTransform();
      }

      glm::mat4 local_transform = transform.getLocalTransform();

      transform.world_transform = parent_transform * local_transform;
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
