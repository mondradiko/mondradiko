// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/instance/ScriptInstance.h"
#include "core/world/Entity.h"

namespace mondradiko {
namespace core {

// Forward declarations
class World;

class ComponentScript : public ScriptInstance {
 public:
  ComponentScript(ScriptEnvironment*, World*, wasm_module_t*, EntityId);

  void update(double);

  // TODO(marceline-cramer) Actually update instance data
  void updateData(const uint8_t*, size_t) {}

  // Publically-accessible World instance
  World* const world;

 private:
  EntityId _self_id;
  uint32_t _this_ptr;
};

}  // namespace core
}  // namespace mondradiko
