// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetHandle.h"
#include "core/assets/ScriptAsset.h"
#include "core/scripting/instance/WorldScript.h"
#include "core/world/Entity.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ComponentScriptEnvironment;
class World;

class ComponentScript : public WorldScript {
 public:
  ComponentScript(ComponentScriptEnvironment*, World*,
                  const AssetHandle<ScriptAsset>&, EntityId,
                  const types::string&);
  ~ComponentScript();

  const AssetHandle<ScriptAsset>& getAsset() { return _asset; }

  void update(double);

  // TODO(marceline-cramer) Actually update instance data
  void updateData(const uint8_t*, size_t) {}

 private:
  AssetHandle<ScriptAsset> _asset;
  types::string _impl;
  EntityId _self_id;
  uint32_t _this_ptr;

  wasm_func_t* _update = nullptr;
};

}  // namespace core
}  // namespace mondradiko
