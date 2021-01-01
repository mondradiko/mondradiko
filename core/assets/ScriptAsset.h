/**
 * @file ScriptAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/AssetPool.h"
#include "core/common/wasm_headers.h"

namespace mondradiko {

// Forward declarations
class ScriptEnvironment;

class ScriptAsset : public Asset {
 public:
  ScriptAsset(assets::ImmutableAsset&, AssetPool*, ScriptEnvironment*);
  ~ScriptAsset();

 private:
  ScriptEnvironment* scripts;

  wasm_module_t* script_module = nullptr;
};

}  // namespace mondradiko
