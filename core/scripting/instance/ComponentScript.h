// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/instance/ScriptInstance.h"
#include "core/world/Entity.h"

namespace mondradiko {
namespace core {

class ComponentScript : public ScriptInstance {
 public:
  ComponentScript(ScriptEnvironment*, wasm_module_t*);

  void update(EntityId, double);

  // TODO(marceline-cramer) Actually update instance data
  void updateData(const uint8_t*, size_t) {}

 private:
};

}  // namespace core
}  // namespace mondradiko
