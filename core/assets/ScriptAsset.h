/**
 * @file ScriptAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
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

namespace mondradiko {

// Forward declarations
class ScriptEnvironment;

class ScriptAsset : public Asset {
 public:
  ScriptAsset(AssetPool*, ScriptEnvironment*);

  // AssetPool implementation
  void load(assets::ImmutableAsset&);
  void unload();

  // TODO(marceline-cramer) Make observers in ScriptEnvironment for events
  // TODO(marceline-cramer) Pass EntityIds to scripts
  // TODO(marceline-cramer) Make ScriptImplementation class to wrap
  // callbacks for each implementation defined in a single ScriptAsset
  void callEvent(const char*);

 private:
  ScriptEnvironment* scripts;

  wasm_module_t* script_module = nullptr;
  wasm_instance_t* module_instance = nullptr;

  std::unordered_map<std::string, wasm_func_t*> event_callbacks;
};

}  // namespace mondradiko
