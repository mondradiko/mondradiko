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

#include <unordered_map>

#include "core/assets/AssetPool.h"
#include "core/world/Entity.h"
#include "flatbuffers/flatbuffers.h"

namespace mondradiko {

// Forward declarations
class Filesystem;
class GpuInstance;
class ScriptEnvironment;

namespace protocol {
struct WorldEvent;

struct SpawnEntity;
struct UpdateComponents;
}  // namespace protocol

class World {
 public:
  World(Filesystem*, GpuInstance*, ScriptEnvironment*);
  ~World();

  void testInitialize();

  //
  // World event callbacks
  //
  void onSpawnEntity(const protocol::SpawnEntity*);
  void onUpdateComponents(const protocol::UpdateComponents*);

  //
  // Helper methods
  //
  bool update();
  void processEvent(const protocol::WorldEvent*);

  template <class ComponentType, class ProtocolComponentType>
  void updateComponents(
      const flatbuffers::Vector<EntityId>*,
      const flatbuffers::Vector<const ProtocolComponentType*>*);

  Filesystem* fs;
  GpuInstance* gpu;
  ScriptEnvironment* scripts;

  // private:
  EntityRegistry registry;
  AssetPool asset_pool;

  std::unordered_map<EntityId, EntityId> server_ids;
};

}  // namespace mondradiko
