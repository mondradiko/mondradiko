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
#include "core/scripting/ScriptEnvironment.h"
#include "core/world/Entity.h"
#include "flatbuffers/flatbuffers.h"

namespace mondradiko {

// Forward declarations
class Filesystem;
class GpuInstance;

namespace protocol {
struct WorldEvent;

struct SpawnEntity;
struct UpdateComponents;
struct UpdateScripts;
}  // namespace protocol

class World {
 public:
  World(Filesystem*, GpuInstance*);
  ~World();

  void testInitialize();

  //
  // World event callbacks
  //
  void onSpawnEntity(const protocol::SpawnEntity*);
  void onUpdateComponents(const protocol::UpdateComponents*);
  void onUpdateScripts(const protocol::UpdateScripts*);

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

  // private:
  EntityRegistry registry;
  AssetPool asset_pool;
  ScriptEnvironment scripts;

  // TODO(marceline-cramer) Remove this "local ID" business, it's kinda
  // pointless
  std::unordered_map<EntityId, EntityId> server_ids;
};

}  // namespace mondradiko
