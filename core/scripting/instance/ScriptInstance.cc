// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/instance/ScriptInstance.h"

#include "core/scripting/environment/ScriptEnvironment.h"
#include "log/log.h"
#include "types/containers/string.h"

namespace mondradiko {
namespace core {

ScriptInstance::ScriptInstance(ScriptEnvironment* scripts,
                               wasm_module_t* script_module)
    : scripts(scripts) {
  wasmtime_error_t* module_error = nullptr;
  wasm_trap_t* module_trap = nullptr;

  types::vector<wasm_extern_t*> module_imports;

  {
    log_zone_named("Link module imports");

    wasm_importtype_vec_t required_imports;
    wasm_module_imports(script_module, &required_imports);

    for (uint32_t i = 0; i < required_imports.size; i++) {
      const wasm_name_t* import_name =
          wasm_importtype_name(required_imports.data[i]);

      // TODO(marceline-cramer) Import other kinds?

      types::string binding_name(import_name->data, import_name->size);
      wasm_func_t* binding_func = scripts->getBinding(binding_name);

      if (binding_func == nullptr) {
        log_err_fmt("Script binding \"%s\" is missing", binding_name.c_str());
        binding_func = scripts->getInterruptFunc();
      }

      module_imports.push_back(wasm_func_as_extern(binding_func));
    }

    wasm_importtype_vec_delete(&required_imports);
  }

  {
    log_zone_named("Create module instance");

    module_error = wasmtime_instance_new(
        scripts->getStore(), script_module, module_imports.data(),
        module_imports.size(), &_module_instance, &module_trap);
    if (scripts->handleError(module_error, module_trap)) {
      log_ftl("Failed to instantiate module");
    }
  }

  {
    log_zone_named("Get module exports");

    wasm_exporttype_vec_t export_types;
    wasm_module_exports(script_module, &export_types);
    wasm_instance_exports(_module_instance, &_instance_externs);

    if (export_types.size != _instance_externs.size) {
      wasm_exporttype_vec_delete(&export_types);
      log_ftl("Mismatch between export_types.size and instance_externs.size");
    }

    for (uint32_t i = 0; i < _instance_externs.size; i++) {
      wasm_exporttype_t* export_type = export_types.data[i];
      const wasm_name_t* export_name = wasm_exporttype_name(export_type);

      wasm_extern_t* exported = _instance_externs.data[i];
      wasm_externkind_t extern_kind = wasm_extern_kind(exported);

      // TODO(marceline-cramer) Handle other kinds of exports
      switch (extern_kind) {
        case WASM_EXTERN_FUNC: {
          wasm_func_t* callback = wasm_extern_as_func(exported);
          types::string callback_name(export_name->data, export_name->size);
          _addCallback(callback_name, callback);

          log_inf_fmt("Imported callback %s", callback_name.c_str());
          log_inf_fmt("Param arity: %zu", wasm_func_param_arity(callback));
          log_inf_fmt("Result arity: %zu", wasm_func_result_arity(callback));

          break;
        }

        case WASM_EXTERN_MEMORY: {
          _memory = wasm_extern_as_memory(exported);
          break;
        }

        default:
          break;
      }
    }

    // TODO(marceline-cramer): Wasmtime-friendly function export handling
    wasm_exporttype_vec_delete(&export_types);
  }

  if (_memory == nullptr) {
    log_wrn("WebAssembly instance does not export its memory");
  }
}

ScriptInstance::~ScriptInstance() {
  if (_module_instance != nullptr) {
    wasm_extern_vec_delete(&_instance_externs);
    wasm_instance_delete(_module_instance);
  }
}

void ScriptInstance::_addCallback(const types::string& callback_name,
                                  wasm_func_t* callback) {
  _callbacks.emplace(callback_name, callback);
}

bool ScriptInstance::_hasCallback(const types::string& callback_name) {
  return _callbacks.find(callback_name) != _callbacks.end();
}

void ScriptInstance::_runCallback(const types::string& callback_name,
                                  const wasm_val_t* args, size_t arg_num,
                                  wasm_val_t* results, size_t result_num) {
  auto iter = _callbacks.find(callback_name);

  if (iter == _callbacks.end()) {
    log_err_fmt("Attempted to run missing callback %s", callback_name.c_str());
    return;
  }

  wasmtime_error_t* module_error = nullptr;
  wasm_trap_t* module_trap = nullptr;

  module_error = wasmtime_func_call(iter->second, args, arg_num, results,
                                    result_num, &module_trap);
  if (scripts->handleError(module_error, module_trap)) {
    log_err_fmt("Error while running callback %s", callback_name.c_str());
  }
}

}  // namespace core
}  // namespace mondradiko
