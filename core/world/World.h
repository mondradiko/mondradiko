// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"
#include "core/physics/Physics.h"
#include "core/scripting/ScriptEnvironment.h"
#include "core/scripting/ScriptObject.h"
#include "core/world/Entity.h"
#include "core/world/IdentifierFactory.h"
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

class World : public StaticScriptObject<World> {
 public:
  World(AssetPool*, Filesystem*, ScriptEnvironment*);
  ~World();

  void initializePrefabs();

  AssetPool* getAssetPool() { return asset_pool; }

  //
  // Entity operations
  //
  void adoptEntity(EntityId, EntityId);
  EntityId createEntity();
  void orphanEntity(EntityId);

  //
  // Query methods
  //
  using EntityList = types::vector<EntityId>;
  size_t getChildren(EntityId, EntityList*);
  size_t getChildrenRecursive(EntityId, EntityList*);

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
  wasm_trap_t* spawnEntity(const wasm_val_t[], wasm_val_t[]);
  wasm_trap_t* spawnEntityAt(const wasm_val_t[], wasm_val_t[]);

  // TODO(marceline-cramer) Blech, restore World privacy
  // Move event callbacks to private
  // private:
  AssetPool* asset_pool;
  Filesystem* fs;
  ScriptEnvironment* scripts;

  EntityRegistry registry;
  IdentifierFactory _id_factory;
  Physics physics;
};

}  // namespace core
}  // namespace mondradiko
