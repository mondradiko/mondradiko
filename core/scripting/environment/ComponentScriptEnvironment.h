// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/Asset.h"
#include "core/scripting/environment/ScriptEnvironment.h"
#include "core/world/Entity.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

// Forward declarations
class AssetPool;
class ComponentScript;
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
   */
  void instantiateScript(EntityId, AssetId, const types::string&);

  // Observer to clean up ScriptComponents
  static void onScriptComponentDestroy(EntityRegistry&, EntityId);

 private:
  AssetPool* const asset_pool;
  World* const world;
};

}  // namespace core
}  // namespace mondradiko
