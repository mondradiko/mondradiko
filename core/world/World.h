// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"
#include "core/physics/Physics.h"
#include "core/scripting/environment/ComponentScriptEnvironment.h"
#include "core/scripting/object/StaticScriptObject.h"
#include "core/world/Entity.h"
#include "lib/include/flatbuffers_headers.h"

namespace mondradiko {

// Forward declarations
namespace protocol {
struct WorldEvent;

struct SpawnEntity;
struct UpdateComponents;
struct UpdateScripts;
}  // namespace protocol

namespace core {

// Forward declarations
class Filesystem;
class WorldScriptEnvironment;

class World : public StaticScriptObject<World> {
 public:
  World(AssetPool*, Filesystem*, WorldScriptEnvironment*);
  ~World();

  void initializePrefabs();

  AssetPool* getAssetPool() { return asset_pool; }

  //
  // Entity operations
  //
  void adopt(EntityId, EntityId);
  void orphan(EntityId);

  //
  // Observer callbacks
  //
  static void onTransformAuthorityConstruct(EntityRegistry&, EntityId);
  static void onTransformAuthorityDestroy(EntityRegistry&, EntityId);

  //
  // World event callbacks
  //
  void onSpawnEntity(const protocol::SpawnEntity*);
  void onUpdateComponents(const protocol::UpdateComponents*);
  void onUpdateScripts(const protocol::UpdateScripts*);

  //
  // Helper methods
  //
  bool update(double);
  void processEvent(const protocol::WorldEvent*);

  template <class ComponentType, class ProtocolComponentType>
  void updateComponents(
      const flatbuffers::Vector<EntityId>*,
      const flatbuffers::Vector<const ProtocolComponentType*>*);

  //
  // Scripting methods
  //
  wasm_trap_t* spawnEntity(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* spawnEntityAt(ScriptInstance*, const wasm_val_t[], wasm_val_t[]);

  // TODO(marceline-cramer) Blech, restore World privacy
  // Move event callbacks to private
  // private:
  AssetPool* asset_pool;
  Filesystem* fs;

  EntityRegistry registry;
  ComponentScriptEnvironment scripts;
  Physics physics;
};

}  // namespace core
}  // namespace mondradiko
