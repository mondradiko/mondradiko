// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/ScriptAsset.h"

#include "core/scripting/ComponentScript.h"
#include "core/scripting/ScriptEnvironment.h"
#include "types/assets/ScriptAsset_generated.h"

namespace mondradiko {

void ScriptAsset::load(const assets::SerializedAsset* asset) {
  const assets::ScriptAsset* script = asset->script();

  if (script->type() == assets::ScriptType::None) {
    log_err("Trying to load a null script");
    return;
  }

  if (script->data()->size() == 0) {
    log_err("Script asset has no contents");
    return;
  }

  // TODO(Turtle1331) free wasmtime memory as early as possible

  wasmtime_error_t* module_error = nullptr;

  wasm_byte_vec_t module_data;
  wasm_byte_vec_new(
      &module_data, script->data()->size(),
      reinterpret_cast<const wasm_byte_t*>(script->data()->data()));

  switch (script->type()) {
    case assets::ScriptType::WasmBinary: {
      module_error = wasmtime_module_new(scripts->getEngine(), &module_data,
                                         &script_module);
      break;
    }

    case assets::ScriptType::WasmText: {
      wasm_byte_vec_t translated_data;

      module_error = wasmtime_wat2wasm(&module_data, &translated_data);

      if (module_error != nullptr) {
        break;
      }

      module_error = wasmtime_module_new(scripts->getEngine(), &translated_data,
                                         &script_module);
      wasm_byte_vec_delete(&translated_data);
      break;
    }

    default: {
      log_ftl_fmt("Unrecognized ScriptAsset type %hhd", script->type());
      break;
    }
  }

  wasm_byte_vec_delete(&module_data);

  if (scripts->handleError(module_error, nullptr)) {
    log_ftl("Failed to compile module");
  }
}

ScriptAsset::~ScriptAsset() {
  if (script_module) wasm_module_delete(script_module);
}

ComponentScript* ScriptAsset::createInstance() const {
  ComponentScript* instance = new ComponentScript(scripts, script_module);
  return instance;
}

}  // namespace mondradiko
