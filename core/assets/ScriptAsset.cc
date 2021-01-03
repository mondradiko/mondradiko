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

#include "assets/format/ScriptAsset.h"
#include "core/scripting/ScriptEnvironment.h"
#include "core/scripting/ScriptInstance.h"

namespace mondradiko {

ScriptAsset::ScriptAsset(AssetPool*, ScriptEnvironment* scripts)
    : scripts(scripts) {}

void ScriptAsset::load(assets::ImmutableAsset& asset) {
  assets::ScriptHeader header;
  asset >> header;

  if (header.type == assets::ScriptType::None) {
    log_err("Trying to load a null script");
    return;
  }

  // TODO(Turtle1331) free wasmtime memory as early as possible

  wasmtime_error_t* module_error = nullptr;

  log_inf("Loading module into memory");
  size_t script_size;
  const char* script_data = asset.getData(&script_size);
  wasm_byte_vec_t module_data;
  wasm_byte_vec_new(&module_data, script_size, script_data);

  log_inf("Compiling module");
  if (header.type == assets::ScriptType::Binary) {
    module_error =
        wasmtime_module_new(scripts->getEngine(), &module_data, &script_module);
  } else if (header.type == assets::ScriptType::Text) {
    wasm_byte_vec_t translated_data;
    wasmtime_wat2wasm(&module_data, &translated_data);
    module_error = wasmtime_module_new(scripts->getEngine(), &translated_data,
                                       &script_module);
    wasm_byte_vec_delete(&translated_data);
  } else {
    log_ftl("Unrecognized ScriptAsset type %d", header.type);
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
