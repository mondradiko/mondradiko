// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/wasm_headers.h"
#include "types/containers/string.h"
#include "types/containers/unordered_map.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ScriptInstance;

using ScriptBindingFactory = wasm_func_t* (*)(ScriptInstance*);

class ScriptEnvironment {
 public:
  ScriptEnvironment();
  ~ScriptEnvironment();

  void linkAssemblyScriptEnv();

  wasm_engine_t* getEngine() { return engine; }
  wasm_store_t* getStore() { return store; }
  wasmtime_interrupt_handle_t* getInterruptHandle() { return interrupt_handle; }

  /**
   * @brief Collects a wasm_func_t, to be destroyed on unload.
   * @param func The wasm_func_t to collect.
   */
  void collectFunc(wasm_func_t*);

  /**
   * @brief Creates a WebAssembly trap with a message.
   * @param message The message for the trap.
   * @return A handle to the new trap.
   */
  wasm_trap_t* createTrap(const types::string&);

  /**
   * @brief Compiles a Wasm module from binary format.
   * @param module_data The Wasm binary data to compile.
   * @return A new wasm_module_t handle.
   */
  wasm_module_t* loadBinaryModule(const wasm_byte_vec_t&);

  /**
   * @brief Compiles a Wasm module from binary format.
   * @param module_data The Wasm binary data to compile.
   * @param data_size The size of the binary data.
   * @return A new wasm_module_t handle.
   */
  wasm_module_t* loadBinaryModule(const char*, size_t);

  /**
   * @brief Compiles a Wasm module from binary format.
   * @param module_data The Wasm binary data to compile.
   * @return A new wasm_module_t handle.
   */
  wasm_module_t* loadBinaryModule(const types::vector<char>&);

  /**
   * @brief Compiles a Wasm module from text format.
   * @param module_data The Wasm text data to compile.
   * @return A new wasm_module_t handle.
   */
  wasm_module_t* loadTextModule(const wasm_byte_vec_t&);

  /**
   * @brief Compiles a Wasm module from binary format.
   * @param module_data The Wasm text data to compile.
   * @param data_size The size of the text data.
   * @return A new wasm_module_t handle.
   */
  wasm_module_t* loadTextModule(const char*, size_t);

  /**
   * @brief Compiles a Wasm module from text format.
   * @param module_data The Wasm text data to compile.
   * @return A new wasm_module_t handle.
   */
  wasm_module_t* loadTextModule(const types::vector<char>&);

  /**
   * @brief Stores a new script object in the script-accessible object registry.
   * @param object_ptr A raw pointer to the object to be stored.
   * @return The new ID of the object.
   */
  uint32_t storeInRegistry(void*);

  /**
   * @brief Retrieves a script object from the registry.
   * @param object_id The ID of the object to be retrieved.
   * @return A raw pointer to the object, or nullptr if the ID was invalid.
   */
  void* getFromRegistry(uint32_t);

  /**
   * @brief Removes a script object from the registry.
   * @param object_id The ID of the object to be removed.
   */
  void removeFromRegistry(uint32_t);

  /**
   * @brief Stores a static script object.
   * @param object_key The symbol of the static object.
   * @param object_ptr The raw pointer to the object.
   * @return False if the object symbol has already been stored, otherwise true.
   */
  bool storeStaticObject(const char*, void*);

  /**
   * @brief Retrieves a static object.
   * @param object_key The symbol of the static object to be retrieved.
   * @return A raw pointer to the object, or nullptr if the key was invalid.
   */
  void* getStaticObject(const char*);

  /**
   * @brief Removes a static object instance, while keeping bindings.
   * @note storeStaticObject() will still return false after removal.
   * @param object_key The symbol of the static object to be removed.
   */
  void removeStaticObject(const char*);

  /**
   * @brief Adds a binding symbol's factory.
   * @param symbol The symbol to link.
   * @param factory The binding factory linked to the symbol.
   */
  void addBindingFactory(const types::string&, ScriptBindingFactory);

  /**
   * @brief Creates a binding associated with a ScriptInstance.
   * @param symbol The symbol of the binding factory.
   * @param instance The ScriptInstance to be bound to.
   * @return A wasm_func_t to bind to the instance.
   */
  wasm_func_t* createBinding(const types::string&, ScriptInstance*);

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
  wasm_func_t* getBinding(const types::string&);

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

  // Callbacks
  static wasm_func_t* abortFactory(ScriptInstance*);
  static wasm_func_t* seedFactory(ScriptInstance*);

  types::vector<wasm_func_t*> func_collection;
  types::vector<void*> object_registry;
  types::unordered_map<types::string, void*> static_objects;
  types::unordered_map<types::string, ScriptBindingFactory> binding_factories;
  types::unordered_map<types::string, wasm_func_t*> bindings;
  wasm_func_t* interrupt_func = nullptr;
};

}  // namespace core
}  // namespace mondradiko
