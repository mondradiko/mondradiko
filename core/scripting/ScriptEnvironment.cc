/**
 * @file ScriptEnvironment.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates an environment for scripts to run in.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/scripting/ScriptEnvironment.h"

#include "core/assets/ScriptAsset.h"
#include "core/bindings/script_linker.h"
#include "core/components/ScriptComponent.h"
#include "log/log.h"

namespace mondradiko {

static wasm_trap_t* interruptCallback(const wasmtime_caller_t* caller,
                                      void* env, const wasm_val_t args[],
                                      wasm_val_t results[]) {
  ScriptEnvironment* scripts = reinterpret_cast<ScriptEnvironment*>(env);
  log_err("Store interrupted");
  wasmtime_interrupt_handle_interrupt(scripts->getInterruptHandle());
  return nullptr;
}

// Dummy finalizer needed for wasmtime_func_new_with_env()
static void interruptCallbackFinalizer(void*) {}

ScriptEnvironment::ScriptEnvironment() {
  log_zone;

  // Create a config to allow interrupts
  wasm_config_t* config = wasm_config_new();
  if (config == nullptr) {
    log_ftl("Failed to create Wasm config");
  }

  wasmtime_config_interruptable_set(config, true);

  // Create the engine
  // Frees the config
  engine = wasm_engine_new_with_config(config);
  if (engine == nullptr) {
    log_ftl("Failed to create Wasm engine");
  }

  // Create the store
  store = wasm_store_new(engine);
  if (store == nullptr) {
    log_ftl("Failed to create Wasm store");
  }

  // Create an interrupt handle
  interrupt_handle = wasmtime_interrupt_handle_new(store);
  if (interrupt_handle == nullptr) {
    log_ftl("Failed to create interrupt handler");
  }

  {
    // Create a function to interrupt the store

    wasm_functype_t* interrupt_func_type = wasm_functype_new_0_0();
    interrupt_func = wasmtime_func_new_with_env(store, interrupt_func_type,
                                                interruptCallback, this,
                                                interruptCallbackFinalizer);
    wasm_functype_delete(interrupt_func_type);
  }

  if (!bindings::linkScriptingApi(this)) {
    log_ftl("Failed to link scripting API.");
  }
}

ScriptEnvironment::~ScriptEnvironment() {
  log_zone;

  for (auto iter : bindings) {
    wasm_func_delete(iter.second);
  }

  if (interrupt_func) wasm_func_delete(interrupt_func);
  if (interrupt_handle) wasmtime_interrupt_handle_delete(interrupt_handle);

  if (store) wasm_store_delete(store);
  if (engine) wasm_engine_delete(engine);
}

void ScriptEnvironment::update(EntityRegistry& registry,
                               AssetPool* asset_pool) {
  auto script_view = registry.view<ScriptComponent>();

  for (auto& e : script_view) {
    auto& script = script_view.get(e);

    if (!script.isLoaded(asset_pool)) continue;

    ScriptAsset& script_asset =
        asset_pool->getAsset<ScriptAsset>(script.getScriptAsset());

    script_asset.callEvent("update");
  }
}

void ScriptEnvironment::addBinding(const char* symbol, wasm_func_t* func) {
  bindings.emplace(std::string(symbol), func);
}

wasm_func_t* ScriptEnvironment::getBinding(const std::string& symbol) {
  auto iter = bindings.find(symbol);
  if (iter == bindings.end()) return nullptr;
  return iter->second;
}

}  // namespace mondradiko
