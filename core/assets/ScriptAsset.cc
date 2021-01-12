/**
 * @file ScriptAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/ScriptAsset.h"

#include <string>
#include <vector>

#include "assets/format/ScriptAsset_generated.h"
#include "core/scripting/ScriptEnvironment.h"
#include "core/scripting/ScriptInstance.h"

namespace mondradiko {

ScriptAsset::ScriptAsset(AssetPool*, ScriptEnvironment* scripts)
    : scripts(scripts) {}

void ScriptAsset::load(const assets::SerializedAsset* asset) {
  const assets::ScriptAsset* script = asset->script();

  if (script->type() == assets::ScriptType::None) {
    log_err("Trying to load a null script");
    return;
  }

  // TODO(Turtle1331) free wasmtime memory as early as possible

  wasmtime_error_t* module_error = nullptr;

  log_inf("Loading module into memory");
  wasm_byte_vec_t module_data;
  wasm_byte_vec_new(
      &module_data, script->data()->size(),
      reinterpret_cast<const wasm_byte_t*>(script->data()->data()));

  log_inf("Compiling module");

  switch (script->type()) {
    case assets::ScriptType::WasmBinary: {
      module_error = wasmtime_module_new(scripts->getEngine(), &module_data,
                                         &script_module);
      break;
    }

    case assets::ScriptType::WasmText: {
      wasm_byte_vec_t translated_data;
      wasmtime_wat2wasm(&module_data, &translated_data);
      module_error = wasmtime_module_new(scripts->getEngine(), &translated_data,
                                         &script_module);
      wasm_byte_vec_delete(&translated_data);
      break;
    }

    default: {
      log_ftl("Unrecognized ScriptAsset type %d", script->type());
      break;
    }
  }

  wasm_byte_vec_delete(&module_data);

  if (scripts->handleError(module_error, nullptr)) {
    log_ftl("Failed to compile module");
  }
}

void ScriptAsset::unload() {
  if (script_module) wasm_module_delete(script_module);

  script_module = nullptr;
}

ScriptInstance* ScriptAsset::createInstance() {
  ScriptInstance* instance = new ScriptInstance(scripts, script_module);
  return instance;
}

}  // namespace mondradiko
