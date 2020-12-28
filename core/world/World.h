/**
 * @file World.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains world configuration, updates and stores Entities, loads
 * models, and receives Events from scripts/network/etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <map>

#include "core/assets/AssetPool.h"

namespace mondradiko {

// Forward declarations
class Filesystem;
class GpuInstance;

namespace protocol {
struct WorldEvent;

struct SpawnEntity;
}  // namespace protocol

// TODO(marceline-cramer) Reflect types from protocol
using EntityId = uint32_t;
static const EntityId NullEntity = 0;
using EntityRegistry = entt::basic_registry<EntityId>;

class World {
 public:
  World(Filesystem*, GpuInstance*);
  ~World();

  //
  // World event callbacks
  //
  void onSpawnEntity(const protocol::SpawnEntity*);

  //
  // Helper methods
  //
  bool update();
  void processEvent(const protocol::WorldEvent*);

  Filesystem* fs;
  GpuInstance* gpu;

  // private:
  EntityRegistry registry;
  AssetPool asset_pool;
};

}  // namespace mondradiko
