// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/ScriptAsset.h"

#include "core/scripting/environment/ScriptEnvironment.h"
#include "core/scripting/instance/ComponentScript.h"
#include "types/assets/ScriptAsset_generated.h"

namespace mondradiko {
namespace core {

ScriptAsset::~ScriptAsset() {
  if (script_module) wasm_module_delete(script_module);
}

bool ScriptAsset::_load(const assets::SerializedAsset* asset) {
  const assets::ScriptAsset* script = asset->script();

  if (script->type() == assets::ScriptType::None) {
    log_err("Trying to load a null script");
    return false;
  }

  if (script->data() != nullptr && script->data()->size() == 0) {
    log_err("Script asset has no contents");
    return false;
  }

  const auto& module_data = script->data();
  switch (script->type()) {
    case assets::ScriptType::WasmBinary: {
      script_module = scripts->loadBinaryModule(
          reinterpret_cast<const char*>(module_data->data()),
          module_data->size());
      break;
    }

    case assets::ScriptType::WasmText: {
      script_module = scripts->loadTextModule(
          reinterpret_cast<const char*>(module_data->data()),
          module_data->size());
      break;
    }

    default: {
      log_ftl_fmt("Unrecognized ScriptAsset type %hhd", script->type());
      break;
    }
  }

  if (script_module == nullptr) {
    log_ftl("Failed to compile module");
  }

  return true;
}

}  // namespace core
}  // namespace mondradiko
