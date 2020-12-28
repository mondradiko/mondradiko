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

#include "core/assets/TextureAsset.h"
#include "core/components/MeshRendererComponent.h"
#include "core/components/TransformComponent.h"
#include "core/filesystem/Filesystem.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"
#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

World::World(Filesystem* fs, GpuInstance* gpu)
    : fs(fs), gpu(gpu), asset_pool(fs) {
  log_zone;
}

World::~World() {
  log_zone;

  asset_pool.unloadAll<MeshAsset>();
  asset_pool.unloadAll<MaterialAsset>();
  asset_pool.unloadAll<TextureAsset>();
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
