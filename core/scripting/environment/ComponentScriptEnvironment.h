// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/Asset.h"
#include "core/scripting/environment/ScriptEnvironment.h"
#include "core/world/Entity.h"

namespace mondradiko {
namespace core {

// Forward declarations
class AssetPool;
class World;

class ComponentScriptEnvironment : public ScriptEnvironment {
 public:
  ComponentScriptEnvironment(AssetPool*, World*);
  ~ComponentScriptEnvironment();

  /**
   * @brief Updates all ScriptComponents in the world.
   * @param dt The update's delta time.
   */
  void update(double);

  /**
   * @brief Updates a local script. Overwrites existing script data, or
   * instantiates a new script if necessary.
   * @param entity The EntityId of the ScriptComponent to update.
   * @param script_id The AssetId of the data's ScriptAsset.
   * @param data A pointer to the script data, or nullptr.
   * @param data_size The length in bytes of the data.
   *
   */
  void updateScript(EntityId, AssetId, const uint8_t*, size_t);

 private:
  AssetPool* const asset_pool;
  World* const world;
};

}  // namespace core
}  // namespace mondradiko
