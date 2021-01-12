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
class ScriptInstance;

class ScriptAsset : public Asset {
 public:
  ScriptAsset(AssetPool*, ScriptEnvironment*);

  // AssetPool implementation
  void load(const assets::SerializedAsset*);
  void unload();

  ScriptInstance* createInstance();

 private:
  ScriptEnvironment* scripts;

  wasm_module_t* script_module = nullptr;
};

}  // namespace mondradiko
