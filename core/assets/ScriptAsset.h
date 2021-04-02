// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"
#include "lib/include/wasm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class ComponentScript;
class ScriptEnvironment;

class ScriptAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::ScriptAsset);

  // Asset lifetime implementation
  explicit ScriptAsset(ScriptEnvironment* scripts) : scripts(scripts) {}
  ~ScriptAsset();

  ComponentScript* createInstance() const;

 protected:
  // Asset implementation
  bool _load(const assets::SerializedAsset*) final;

 private:
  ScriptEnvironment* scripts;

  wasm_module_t* script_module = nullptr;
};

}  // namespace core
}  // namespace mondradiko
