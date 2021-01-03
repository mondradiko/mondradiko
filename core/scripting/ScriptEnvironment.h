/**
 * @file ScriptEnvironment.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates an environment for scripts to run in.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <unordered_map>

#include "core/assets/AssetPool.h"
#include "core/common/wasm_headers.h"
#include "core/world/Entity.h"

namespace mondradiko {

// Forward declarations
class World;

class ScriptEnvironment {
 public:
  ScriptEnvironment();
  ~ScriptEnvironment();

  void linkComponentApis(World*);
  void update(EntityRegistry&, AssetPool*);

  wasm_engine_t* getEngine() { return engine; }
  wasm_store_t* getStore() { return store; }
  wasmtime_interrupt_handle_t* getInterruptHandle() { return interrupt_handle; }

  /**
   * @brief Updates a local script. Overwrites existing script data, or
   * instantiates a new script if necessary.
   * @param registry The World's registry.
   * @param asset_pool The World's asset pool.
   * @param entity The EntityId of the ScriptComponent to update.
   * @param script_asset The AssetId of the data's ScriptAsset.
   * @param data A pointer to the script data, or nullptr.
   * @param data_size The length in bytes of the data.
   *
   */
  void updateScript(EntityRegistry&, AssetPool*, EntityId, AssetId, const uint8_t*, size_t);

  /**
   * @brief Adds a binding symbol's callback to the ScriptEnvironment.
   *
   * @param symbol The name to link.
   * @param func The function linked to the symbol.
   *
   */
  void addBinding(const char*, wasm_func_t*);

  /**
   * @brief Attempts to look up a stored binding.
   * @param symbol The binding to search for.
   *
   * @return The binding's Wasm function, or nullptr if it's not found.
   */
  wasm_func_t* getBinding(const std::string&);

  /**
   * @brief Gets a Wasm function that interrupts the store when called.
   * Used by ScriptAsset to replace missing binding imports.
   *
   * @return A Wasm host-defined function that interrupts the store
   * when called.
   */
  wasm_func_t* getInterruptFunc() { return interrupt_func; }

  /**
   * @brief Tests if Wasmtime errors were thrown, and logs them if they were.
   * @param error Wasmtime error to log, or nullptr if none was thrown.
   * @param trap Wasmtime trap to log, or nullptr if none was thrown.
   *
   * @return True if an error occurred, false if it didn't.
   */
  bool handleError(wasmtime_error_t*, wasm_trap_t*);

 private:
  wasm_engine_t* engine = nullptr;
  wasm_store_t* store = nullptr;
  wasmtime_interrupt_handle_t* interrupt_handle = nullptr;

  std::unordered_map<std::string, wasm_func_t*> bindings;
  wasm_func_t* interrupt_func = nullptr;
};

}  // namespace mondradiko
