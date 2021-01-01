/**
 * @file ScriptEnvironment.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates an environment for scripts to run in.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/scripting/ScriptEnvironment.h"

#include "bindings/script_linker.h"
#include "log/log.h"

namespace mondradiko {

ScriptEnvironment::ScriptEnvironment() {
  log_zone;

  engine = wasm_engine_new();
  store = wasm_store_new(engine);

  if(!bindings::linkScriptingApi(this)) {
    log_ftl("Failed to link scripting API.");
  }
}

ScriptEnvironment::~ScriptEnvironment() {
  log_zone;

  if (store) wasm_store_delete(store);
  if (engine) wasm_engine_delete(engine);
}

void ScriptEnvironment::update(EntityRegistry& registry) {}

}  // namespace mondradiko
