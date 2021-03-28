// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/ComponentScript.h"

#include <array>

#include "core/scripting/ScriptEnvironment.h"

namespace mondradiko {
namespace core {

ComponentScript::ComponentScript(ScriptEnvironment* scripts,
                                 wasm_module_t* module)
    : ScriptInstance(scripts, module) {}

void ComponentScript::update(EntityId self_id, double dt) {
  if (!_hasCallback("update")) {
    log_wrn("Skipping update");
    return;
  }

  std::array<wasm_val_t, 2> args;

  auto& self_arg = args[0];
  self_arg.kind = WASM_I32;
  self_arg.of.i32 = self_id;

  auto& dt_arg = args[1];
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;

  _runCallback("update", args.data(), args.size(), nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
