// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/instance/UiScript.h"

#include "core/ui/UiPanel.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

UiScript::UiScript(ScriptEnvironment* scripts, wasm_module_t* module)
    : ScriptInstance(scripts, module) {}

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

}  // namespace core
}  // namespace mondradiko
