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

  MeshRendererComponent mesh_renderer_component{
      .mesh_asset = asset_pool.loadAsset<MeshAsset>(0x84b42359, gpu),
      .material_asset = asset_pool.loadAsset<MaterialAsset>(0xf643d4dc, gpu)};

  ScriptComponent script_component{
      .script_asset = asset_pool.loadAsset<ScriptAsset>(0x31069ecf, scripts)};

  TransformComponent transform_component{
    .parent = NullEntity,
    .local_transform = glm::mat4(1.0)
  };

  registry.emplace<MeshRendererComponent>(test_entity, mesh_renderer_component);
  registry.emplace<ScriptComponent>(test_entity, script_component);
  registry.emplace<TransformComponent>(test_entity, transform_component);
}

///////////////////////////////////////////////////////////////////////////////
// World event callbacks
///////////////////////////////////////////////////////////////////////////////

void World::onSpawnEntity(const protocol::SpawnEntity* event) {}

///////////////////////////////////////////////////////////////////////////////
// Helper methods
///////////////////////////////////////////////////////////////////////////////

bool World::update() {
  log_zone;

  {
    log_zone_named("Update TransformComponent self-reference");

    auto transform_view = registry.view<TransformComponent>();

    for (EntityId e : transform_view) {
      transform_view.get(e).this_entity = e;
    }
  }

  {
    log_zone_named("Sort transform hierarchy");

    registry.sort<TransformComponent>(
        [](const auto& parent, const auto& child) {
          // Sort children after parents
          return parent.this_entity >= child.parent;
        });
  }

  {
    log_zone_named("Transform children under parents");

    auto transform_view = registry.view<TransformComponent>();

    for (EntityId e : transform_view) {
      TransformComponent& transform = transform_view.get(e);

      glm::mat4 parent_transform;
      if (transform.parent == NullEntity) {
        parent_transform = glm::mat4(1.0);
      } else {
        parent_transform =
            registry.get<TransformComponent>(transform.parent).world_transform;
      }

      transform.world_transform = parent_transform * transform.local_transform;
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

    default: {
      log_err("Unrecognized world event %d", event->type());
      break;
    }
  }
}

}  // namespace mondradiko
