/**
 * @file World.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains World configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/world/World.h"

#include <iostream>

#include "core/assets/ScriptAsset.h"
#include "core/assets/TextureAsset.h"
#include "core/components/MeshRendererComponent.h"
#include "core/components/ScriptComponent.h"
#include "core/components/TransformComponent.h"
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "core/scripting/ScriptEnvironment.h"
#include "log/log.h"
#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

World::World(Filesystem* fs, GpuInstance* gpu)
    : fs(fs), gpu(gpu), asset_pool(fs) {
  log_zone;

  if (gpu) {
    asset_pool.initializeAssetType<MaterialAsset>(gpu);
    asset_pool.initializeAssetType<MeshAsset>(gpu);
    asset_pool.initializeAssetType<TextureAsset>(gpu);
  } else {
    asset_pool.initializeDummyAssetType<MaterialAsset>(gpu);
    asset_pool.initializeDummyAssetType<MeshAsset>(gpu);
    asset_pool.initializeDummyAssetType<TextureAsset>(gpu);
  }

  asset_pool.initializeAssetType<ScriptAsset>(&scripts);

  scripts.linkComponentApis(this);
}

World::~World() {
  log_zone;

  asset_pool.unloadAll<MaterialAsset>();
  asset_pool.unloadAll<MeshAsset>();
  asset_pool.unloadAll<ScriptAsset>();
  asset_pool.unloadAll<TextureAsset>();
}

void World::testInitialize() {
  auto test_entity = registry.create();

  MeshRendererComponent mesh_renderer_component(
      static_cast<AssetId>(0x4c8d6924), static_cast<AssetId>(0x95414060));
  TransformComponent transform_component;

  registry.emplace<MeshRendererComponent>(test_entity, mesh_renderer_component);
  registry.emplace<TransformComponent>(test_entity, transform_component);

  // Update an entity's script to initialize the ScriptComponent
  /*scripts.updateScript(registry, &asset_pool, test_entity, 0x55715294,
     nullptr, static_cast<size_t>(0));*/
}

///////////////////////////////////////////////////////////////////////////////
// World event callbacks
///////////////////////////////////////////////////////////////////////////////

void World::onSpawnEntity(const protocol::SpawnEntity* event) {
  EntityId id = static_cast<EntityId>(event->new_id());

  if (registry.valid(id)) {
    log_wrn("Spawned entity's ID is taken: 0x%0lx", id);
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

    case protocol::ComponentType::TransformComponent: {
      updateComponents<TransformComponent>(entities,
                                           update_components->transform());
      break;
    }

    default: {
      log_err("Unrecognized component type %d", update_components->type());
      break;
    }
  }
}

void World::onUpdateScripts(const protocol::UpdateScripts* update_scripts) {}

///////////////////////////////////////////////////////////////////////////////
// Helper methods
///////////////////////////////////////////////////////////////////////////////

bool World::update() {
  log_zone;

  {
    log_zone_named("Update TransformComponent self-reference");

    auto transform_view = registry.view<TransformComponent>();

    for (EntityId e : transform_view) {
      TransformComponent& transform = transform_view.get(e);

      transform.this_entity = e;
    }
  }

  {
    log_zone_named("Sort transform hierarchy");

    registry.sort<TransformComponent>(
        [](const auto& parent, const auto& child) {
          // Sort children after parents
          return parent.this_entity >= child.getParent();
        });
  }

  {
    log_zone_named("Transform children under parents");

    auto transform_view = registry.view<TransformComponent>();

    for (EntityId e : transform_view) {
      TransformComponent& transform = transform_view.get(e);

      auto parent = transform.getParent();
      glm::mat4 parent_transform;
      if (parent == NullEntity) {
        parent_transform = glm::mat4(1.0);
      } else {
        parent_transform =
            registry.get<TransformComponent>(parent).world_transform;
      }

      glm::mat4 local_transform = transform.getLocalTransform();

      transform.world_transform = parent_transform * local_transform;
    }
  }

  scripts.update(registry, &asset_pool);

  FrameMark;
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
      log_err("Unrecognized world event %d", event->type());
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
      log_wrn("Entity hasn't been initialized client-side: 0x%0lx", id);
      // Discard returned ID, since we just ensured that the hint will be used
      static_cast<void>(registry.create(id));
    }

    const ProtocolComponentType& component = *components->Get(i);

    if (registry.has<ComponentType>(id)) {
      registry.get<ComponentType>(id).writeData(component);
    } else {
      registry.emplace<ComponentType>(id, component);
    }
  }
}

}  // namespace mondradiko
