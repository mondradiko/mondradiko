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

 private:
  wasm_engine_t* engine = nullptr;
  wasm_store_t* store = nullptr;
};

}  // namespace mondradiko
