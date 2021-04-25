// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/instance/UiScript.h"

#include "core/ui/UiPanel.h"
#include "log/log.h"

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

void UiScript::handleMessage(const types::string& message) {
  uint32_t message_ptr;
  if (!AS_newString(message, &message_ptr)) {
    log_err("Failed to display message");
    return;
  }

  wasm_val_t message_arg;
  message_arg.kind = WASM_I32;
  message_arg.of.i32 = message_ptr;
  runCallback("handleMessage", &message_arg, 1, nullptr, 0);
}

void UiScript::update(double dt) {
  wasm_val_t dt_arg;
  dt_arg.kind = WASM_F64;
  dt_arg.of.f64 = dt;
  _runCallback("update", &dt_arg, 1, nullptr, 0);
}

}  // namespace core
}  // namespace mondradiko
