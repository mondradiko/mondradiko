/**
 * @file ScriptAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/ScriptAsset.h"

#include <string>
#include <vector>

#include "assets/format/ScriptAsset.h"
#include "core/scripting/ScriptEnvironment.h"

namespace mondradiko {

// Helper to handle Wasm errors
void exit_with_error(const char* message, wasmtime_error_t* error,
                     wasm_trap_t* trap) {
  wasm_byte_vec_t error_message;
  if (error != NULL) {
    wasmtime_error_message(error, &error_message);
    wasmtime_error_delete(error);
  } else {
    wasm_trap_message(trap, &error_message);
    wasm_trap_delete(trap);
  }
  std::string error_string(error_message.data, error_message.size);
  wasm_byte_vec_delete(&error_message);
  log_err("%s: %s", message, error_string);
}

ScriptAsset::ScriptAsset(assets::ImmutableAsset& asset, AssetPool*,
                         ScriptEnvironment* scripts)
    : scripts(scripts) {
  assets::ScriptHeader header;
  asset >> header;

  if (header.type == assets::ScriptType::None) {
    log_err("Trying to load a null script");
    return;
  }

  // TODO(Turtle1331) free wasmtime memory as early as possible
  // TODO(Turtle1331) debug segfault on missing import

  wasmtime_error_t* module_error = NULL;
  wasm_trap_t* module_trap = NULL;

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

  if (module_error != NULL) {
    exit_with_error("Failed to compile module:", module_error, NULL);
  }

  log_inf("Creating module callbacks");

  wasm_importtype_vec_t required_imports;
  wasm_module_imports(script_module, &required_imports);

  std::vector<wasm_extern_t*> module_imports;

  for (uint32_t i = 0; i < required_imports.size; i++) {
    const wasm_name_t* import_name =
        wasm_importtype_name(required_imports.data[i]);

    // TODO(marceline-cramer) Import other kinds?

    std::string binding_name(import_name->data, import_name->size);
    wasm_func_t* binding_func = scripts->getBinding(binding_name);

    if (binding_func) {
      module_imports.push_back(wasm_func_as_extern(binding_func));
    } else {
      log_err("Script binding \"%s\" is missing", binding_name.c_str());
      // HACK(marceline-cramer) This will segfault until we can fail safely
      module_imports.push_back(nullptr);
    }
  }

  log_inf("Instantiating module");
  wasm_instance_t* module_instance = NULL;
  module_error = wasmtime_instance_new(
      scripts->getStore(), script_module, module_imports.data(),
      module_imports.size(), &module_instance, &module_trap);
  if (module_instance == NULL) {
    exit_with_error("Failed to instantiate module", module_error, module_trap);
  }

  log_inf("Extracting exported functions");
  wasm_extern_vec_t instance_externs;
  wasm_instance_exports(module_instance, &instance_externs);
  assert(instance_externs.size == 1);
  wasm_func_t* run_f = wasm_extern_as_func(instance_externs.data[0]);
  assert(run_f != NULL);

  log_inf("Calling exported function");
  module_error = wasmtime_func_call(run_f, NULL, 0, NULL, 0, &module_trap);
  wasm_extern_vec_delete(&instance_externs);
  wasm_instance_delete(module_instance);
  if (module_error != NULL || module_trap != NULL) {
    exit_with_error("Error while running module", module_error, module_trap);
  }
}

ScriptAsset::~ScriptAsset() {
  if (script_module) wasm_module_delete(script_module);
}

}  // namespace mondradiko
