/**
 * @file ScriptInstance.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Instantiates the Wasm module imported from a ScriptAsset.
 * @date 2021-01-02
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/scripting/ScriptInstance.h"

#include <vector>

#include "core/scripting/ScriptEnvironment.h"
#include "log/log.h"

namespace mondradiko {

ScriptInstance::ScriptInstance(ScriptEnvironment* scripts,
                               wasm_module_t* script_module)
    : scripts(scripts) {
  wasmtime_error_t* module_error = nullptr;
  wasm_trap_t* module_trap = nullptr;

  {
    log_zone_named("Create module callbacks");

    wasm_importtype_vec_t required_imports;
    wasm_module_imports(script_module, &required_imports);

    std::vector<wasm_extern_t*> module_imports;

    for (uint32_t i = 0; i < required_imports.size; i++) {
      const wasm_name_t* import_name =
          wasm_importtype_name(required_imports.data[i]);

      // TODO(marceline-cramer) Import other kinds?

      std::string binding_name(import_name->data, import_name->size);
      wasm_func_t* binding_func = scripts->getBinding(binding_name);

      if (binding_func == nullptr) {
        log_err("Script binding \"%s\" is missing", binding_name.c_str());
        binding_func = scripts->getInterruptFunc();
      }

      module_imports.push_back(wasm_func_as_extern(binding_func));
    }
  }

  std::vector<wasm_extern_t*> module_imports;

  {
    log_zone_named("Link module imports");

    wasm_importtype_vec_t required_imports;
    wasm_module_imports(script_module, &required_imports);

    for (uint32_t i = 0; i < required_imports.size; i++) {
      const wasm_name_t* import_name =
          wasm_importtype_name(required_imports.data[i]);

      // TODO(marceline-cramer) Import other kinds?

      std::string binding_name(import_name->data, import_name->size);
      wasm_func_t* binding_func = scripts->getBinding(binding_name);

      if (binding_func == nullptr) {
        log_err("Script binding \"%s\" is missing", binding_name.c_str());
        binding_func = scripts->getInterruptFunc();
      }

      module_imports.push_back(wasm_func_as_extern(binding_func));
    }

    {
      log_zone_named("Create module instance");

      module_error = wasmtime_instance_new(
          scripts->getStore(), script_module, module_imports.data(),
          module_imports.size(), &module_instance, &module_trap);
      if (scripts->handleError(module_error, nullptr)) {
        log_ftl("Failed to instantiate module");
      }
    }

    {
      log_zone_named("Get module exports");

      wasm_extern_vec_t instance_externs;
      wasm_instance_exports(module_instance, &instance_externs);

      /*for (uint32_t i = 0; i < instance_externs.size; i++) {
        const wasm_extern_t* exported = instance_externs.data[i];

        wasm_externkind_t exported_kind = wasm_extern_kind(exported);

        wasm_name_t export_name;
        wasm_exporttype_t* export_type =
            wasm_exporttype_new(&export_name, exported);

        // TODO(marceline-cramer) Handle other kinds of exports
        if (exported_kind == WASM_EXTERN_FUNC) {
        }
        const wasm_name_t* callback_name = wasm_exporttype_name(exported_type);
      }*/

      if (instance_externs.size < 1) {
        log_ftl("Script module has no exports");
      }

      // TODO(marceline-cramer) Register callbacks under their exported names
      wasm_func_t* update_func = wasm_extern_as_func(instance_externs.data[0]);
      addCallback("update", update_func);
    }
  }
}

ScriptInstance::~ScriptInstance() {
  if (module_instance) wasm_instance_delete(module_instance);
}

void ScriptInstance::addCallback(const std::string& callback_name,
                                 wasm_func_t* callback) {
  callbacks.emplace(callback_name, callback);
}

void ScriptInstance::runCallback(const std::string& callback_name) {
  auto iter = callbacks.find(callback_name);

  if (iter == callbacks.end()) {
    log_err("Attempted to call non-existent callback %s", callback_name);
    return;
  }

  wasmtime_error_t* module_error = nullptr;
  wasm_trap_t* module_trap = nullptr;

  module_error =
      wasmtime_func_call(iter->second, nullptr, 0, nullptr, 0, &module_trap);
  if (scripts->handleError(module_error, module_trap)) {
    log_err("Error while running callback %s", callback_name.c_str());
  }
}

}  // namespace mondradiko
