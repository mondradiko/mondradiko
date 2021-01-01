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

#include "core/bindings/script_linker.h"
#include "log/log.h"
#include <wasm.h>

namespace mondradiko {

wasm_config_t* ScriptEnvironment::config = nullptr;

ScriptEnvironment::ScriptEnvironment() {
  log_zone;

  // Create a config to allow interrupts, if one doesn't exist already
  if (config == NULL) {
    config = wasm_config_new();
    assert(config != NULL);
    wasmtime_config_interruptable_set(config, true);
  }

  // Create the engine and store
  engine = wasm_engine_new_with_config(config);
  assert(engine != NULL);
  store = wasm_store_new(engine);
  assert(store != NULL);

  // Create an interrupt handle
  wasmtime_interrupt_handle_t *interrupt_handle = wasmtime_interrupt_handle_new(store);
  assert(interrupt_handle != NULL);

  if (!bindings::linkScriptingApi(this)) {
    log_ftl("Failed to link scripting API.");
  }
}

ScriptEnvironment::~ScriptEnvironment() {
  log_zone;

  for (auto iter : bindings) {
    wasm_func_delete(iter.second);
  }

  if (store) wasm_store_delete(store);
  if (engine) wasm_engine_delete(engine);
  if (interrupt_handle) wasmtime_interrupt_handle_delete(interrupt_handle);
}

void ScriptEnvironment::update(EntityRegistry& registry) {}

void ScriptEnvironment::addBinding(const char* symbol, wasm_func_t* func) {
  bindings.emplace(std::string(symbol), func);
}

wasm_func_t* ScriptEnvironment::getBinding(const std::string& symbol) {
  auto iter = bindings.find(symbol);
  if (iter == bindings.end()) return nullptr;
  return iter->second;
}

}  // namespace mondradiko
