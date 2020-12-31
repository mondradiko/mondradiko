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

World::World(Filesystem* fs, GpuInstance* gpu, ScriptEnvironment* scripts)
    : fs(fs), gpu(gpu), scripts(scripts), asset_pool(fs) {
  log_zone;
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

#ifdef TEST_INITIALIZE
  MeshRendererComponent mesh_renderer_component{
      .mesh_asset = asset_pool.loadAsset<MeshAsset>(0x84b42359, gpu),
      .material_asset = asset_pool.loadAsset<MaterialAsset>(0xf643d4dc, gpu)};

  ScriptComponent script_component{
      .script_asset = asset_pool.loadAsset<ScriptAsset>(0x31069ecf, scripts)};
#endif

  TransformComponent transform_component;

#ifdef TEST_INITIALIZE
  registry.emplace<MeshRendererComponent>(test_entity,
  mesh_renderer_component); registry.emplace<ScriptComponent>(test_entity,
  script_component);
#endif
  registry.emplace<TransformComponent>(test_entity, transform_component);
}

///////////////////////////////////////////////////////////////////////////////
// World event callbacks
///////////////////////////////////////////////////////////////////////////////

void World::onSpawnEntity(const protocol::SpawnEntity* event) {
  EntityId server_id = static_cast<EntityId>(event->new_id());

  if (server_ids.find(server_id) != server_ids.end()) {
    return;
  }

  EntityId new_id = registry.create(server_id);
  server_ids.emplace(server_id, new_id);
}

void World::onUpdateComponents(
    const protocol::UpdateComponents* update_components) {
  auto entities = update_components->entities();

  switch (update_components->type()) {
    case protocol::ComponentType::NoComponent: {
      log_dbg("Received empty component update");
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

      // TODO(marceline-cramer) Networked transform hierarchies
      transform.local_parent = NullEntity;
      transform.this_entity = e;
    }
  }

  {
    log_zone_named("Sort transform hierarchy");

    registry.sort<TransformComponent>(
        [](const auto& parent, const auto& child) {
          // Sort children after parents
          return parent.this_entity >= child.local_parent;
        });
  }

  {
    log_zone_named("Transform children under parents");

    auto transform_view = registry.view<TransformComponent>();

    for (EntityId e : transform_view) {
      TransformComponent& transform = transform_view.get(e);

      glm::mat4 parent_transform;
      if (transform.local_parent == NullEntity) {
        parent_transform = glm::mat4(1.0);
      } else {
        parent_transform =
            registry.get<TransformComponent>(transform.local_parent)
                .world_transform;
      }

      glm::mat4 local_transform = transform.getLocalTransform();

      transform.world_transform = parent_transform * local_transform;
    }
  }

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
    EntityId server_entity = entities->Get(i);

    auto it = server_ids.find(server_entity);
    EntityId client_entity;
    if (it == server_ids.end()) {
      client_entity = registry.create();
      server_ids.emplace(server_entity, client_entity);
    } else {
      client_entity = it->second;
    }

    const ProtocolComponentType& component = *components->Get(i);

    if (registry.has<ComponentType>(client_entity)) {
      registry.get<ComponentType>(client_entity).writeData(component);
    } else {
      registry.emplace<ComponentType>(client_entity, component);
    }
  }
}

}  // namespace mondradiko
