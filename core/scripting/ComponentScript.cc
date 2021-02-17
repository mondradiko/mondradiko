// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/ComponentScript.h"

#include "core/scripting/ScriptEnvironment.h"

namespace mondradiko {

ComponentScript::ComponentScript(ScriptEnvironment* scripts,
                                 wasm_module_t* module)
    : ScriptInstance(scripts, module) {
  if (_hasCallback("update")) {
    update_func = _getCallback("update");
  }
}

void ComponentScript::update(EntityId self_id) {
  if (update_func == nullptr) return;

  wasmtime_error_t* module_error = nullptr;
  wasm_trap_t* module_trap = nullptr;

  wasm_val_t self_param;
  self_param.kind = WASM_I32;
  self_param.of.i32 = self_id;

  module_error =
      wasmtime_func_call(update_func, &self_param, 1, nullptr, 0, &module_trap);
  if (scripts->handleError(module_error, module_trap)) {
    log_err_fmt("Error while running update()");
  }
}

}  // namespace mondradiko
