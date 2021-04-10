// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/UiScript.h"

#include "core/ui/UiPanel.h"

namespace mondradiko {
namespace core {

UiScript::UiScript(ScriptEnvironment* scripts, wasm_module_t* module)
    : ScriptInstance(scripts, module) {}

void UiScript::update(double dt) {
  wasm_val_t dt_arg;
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;
  _runCallback("update", &dt_arg, 1, nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
