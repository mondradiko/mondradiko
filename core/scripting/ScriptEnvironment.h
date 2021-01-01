/**
 * @file ScriptEnvironment.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates an environment for scripts to run in.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <unordered_map>

#include "core/common/wasm_headers.h"
#include "core/world/Entity.h"

namespace mondradiko {

class ScriptEnvironment {
 public:
  ScriptEnvironment();
  ~ScriptEnvironment();

  void update(EntityRegistry&);

  wasm_store_t* getStore() { return store; }
  wasm_engine_t* getEngine() { return engine; }

  template <class ComponentType>
  ComponentType* getComponent() {
    // TODO(marceline-cramer) Add ComponentId
    // TODO(marceline-cramer) Implement this
    return nullptr;
  }

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

 private:
  wasm_engine_t* engine = nullptr;
  wasm_store_t* store = nullptr;

  std::unordered_map<std::string, wasm_func_t*> bindings;
};

}  // namespace mondradiko
