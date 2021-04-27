// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/UiScript.h"

#include <array>

#include "core/ui/UiPanel.h"

namespace mondradiko {
namespace core {

UiScript::UiScript(ScriptEnvironment* scripts, wasm_module_t* module)
    : ScriptInstance(scripts, module) {}

uint32_t UiScript::bindPanel(UiPanel* panel) {
  wasm_val_t arg;
  arg.kind = WASM_I32;
  arg.of.i32 = panel->getObjectKey();

  wasm_val_t result;
  _runCallback("createPanel", &arg, 1, &result, 1);
  return result.of.i32;
}

void UiScript::selectAt(const glm::vec2& coords) {
  std::array<wasm_val_t, 2> args;

  auto& x_arg = args[0];
  x_arg.kind = WASM_F64;
  x_arg.of.f64 = coords.x;

  auto& y_arg = args[1];
  y_arg.kind = WASM_F64;
  y_arg.of.f64 = coords.y;

  _runCallback("selectAt", args.data(), args.size(), nullptr, 0);
}

void UiScript::update(double dt) {
  wasm_val_t dt_arg;
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;
  _runCallback("update", &dt_arg, 1, nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
