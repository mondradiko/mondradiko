// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/instance/ComponentScript.h"

#include <array>

#include "core/scripting/environment/ScriptEnvironment.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

ComponentScript::ComponentScript(ScriptEnvironment* scripts, World* world,
                                 wasm_module_t* module, EntityId self_id)
    : ScriptInstance(scripts, module), world(world), _self_id(self_id) {
  wasm_val_t self_arg;
  self_arg.kind = WASM_I32;
  self_arg.of.i32 = _self_id;

  wasm_val_t this_result;

  runCallback("instantiate", &self_arg, 1, &this_result, 1);

  _this_ptr = this_result.of.i32;

  AS_pin(_this_ptr);
}

void ComponentScript::update(double dt) {
  if (!hasCallback("update")) {
    log_wrn("Skipping update");
    return;
  }

  std::array<wasm_val_t, 2> args;

  auto& this_arg = args[0];
  this_arg.kind = WASM_I32;
  this_arg.of.i32 = _this_ptr;

  auto& dt_arg = args[1];
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;

  runCallback("update", args.data(), args.size(), nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
