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
  ComponentScript(ScriptEnvironment*, World*, wasm_module_t*);

  void update(EntityId, double);

  // TODO(marceline-cramer) Actually update instance data
  void updateData(const uint8_t*, size_t) {}

  // Publically-accessible World instance
  World* const world;

 private:
};

}  // namespace core
}  // namespace mondradiko
