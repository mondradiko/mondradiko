// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/scripting/ScriptInstance.h"
#include "core/world/Entity.h"

namespace mondradiko {

class ComponentScript : public ScriptInstance {
 public:
  ComponentScript(ScriptEnvironment*, wasm_module_t*);

  void update(EntityId);

  // TODO(marceline-cramer) Actually update instance data
  void updateData(const uint8_t*, size_t) {}

 private:
  wasm_func_t* update_func = nullptr;
};

}  // namespace mondradiko
