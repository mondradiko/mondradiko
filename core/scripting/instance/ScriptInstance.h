// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later
//
// After a Wasm module is loaded and created by ScriptAsset, it can create
// ScriptInstances, which represent individual WebAssembly processes, each with
// their own execution environment.
//
// When a ScriptInstance is created, it calls the instantiate() function of the
// instance, if it exists. If it does, then instantiate() returns a pointer to a
// structure inside of Wasm memory, that needs to be synchronized over the
// network. This is represented by a byte array, copied directly from the Wasm
// store, and packaged in the ScriptComponent flatbuffer.
//
// Then, the callbacks exported from from the module are registered, and the
// ScriptEnvironment is ready to start calling them on world events.
//
///////////////////////////////////////////////////////////////////////////////
// To-do list:
//
// TODO(marceline-cramer) Actually implement dynamic allocation
// TODO(marceline-cramer) Find way to refcount ScriptInstances from ScriptAsset
// so that ScriptAssets aren't unloaded before ScriptInstances are
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/world/Entity.h"
#include "lib/include/wasm_headers.h"
#include "types/containers/string.h"
#include "types/containers/unordered_map.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ScriptEnvironment;

class ScriptInstance {
 public:
  ScriptInstance(ScriptEnvironment*, wasm_module_t*);
  ~ScriptInstance();

  // TODO(marceline-cramer) Make observers in ScriptEnvironment for events
  // TODO(marceline-cramer) Define entrypoint classes and their sizes

 protected:
  ScriptEnvironment* scripts;

  //////////////////////////////////////////////////////////////////////////////
  // Callback helpers
  //////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Adds a callback.
   * @param symbol The symbol of the callback.
   * @param func The callback's function.
   */
  void _addCallback(const types::string&, wasm_func_t*);

  /**
   * @brief Checks if a callback is available.
   * @param symbol The symbol of the callback.
   * @return True if the callback is available, false if not.
   */
  bool _hasCallback(const types::string&);

  /**
   * @brief Retrieves a callback's function.
   * @param symbol The symbol of the callback.
   * @return The wasm_func_t of the callback, or nullptr if unavailable.
   */
  wasm_func_t* _getCallback(const types::string&);

  /**
   * @brief Runs a callback, while performing proper error checking.
   * @param symbol The symbol of the callback.
   * @param args A pointer to an array of arguments.
   * @param arg_num The number of arguments.
   * @param results A pointer to an array of results.
   * @param result_num The number of results.
   * @return True on success, false on an invalid callback or a trap throw.
   */
  bool _runCallback(const types::string&, const wasm_val_t*, size_t,
                    wasm_val_t*, size_t);

  /**
   * @brief Runs a function directly, while performing proper error checking.
   * @param func The function to call.
   * @param args A pointer to an array of arguments.
   * @param arg_num The number of arguments.
   * @param results A pointer to an array of results.
   * @param result_num The number of results.
   * @return True on success, false on an invalid callback or a trap throw.
   */
  bool _runFunction(wasm_func_t*, const wasm_val_t*, size_t, wasm_val_t*,
                    size_t);

 private:
  wasm_instance_t* _module_instance = nullptr;
  wasm_extern_vec_t _instance_externs;

  // Exported data
  wasm_memory_t* _memory = nullptr;
  types::unordered_map<types::string, wasm_func_t*> _callbacks;
};

}  // namespace core
}  // namespace mondradiko
