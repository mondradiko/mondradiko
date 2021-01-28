// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string>
#include <unordered_map>

#include "core/assets/AssetPool.h"
#include "lib/include/wasm_headers.h"

namespace mondradiko {

// Forward declarations
class ScriptEnvironment;
class ScriptInstance;

class ScriptAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::ScriptAsset);

  // Asset lifetime implementation
  explicit ScriptAsset(ScriptEnvironment* scripts) : scripts(scripts) {}
  void load(const assets::SerializedAsset*) final;
  ~ScriptAsset();

  ScriptInstance* createInstance() const;

 private:
  ScriptEnvironment* scripts;

  wasm_module_t* script_module = nullptr;
};

}  // namespace mondradiko
