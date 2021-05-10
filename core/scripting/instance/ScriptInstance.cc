// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/scripting/instance/ScriptInstance.h"

#include <codecvt>
#include <locale>

#include "core/scripting/environment/ScriptEnvironment.h"
#include "log/log.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

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
      wasm_func_t* binding_func = scripts->createBinding(binding_name, this);

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
          addCallback(callback_name, callback);
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

  _new_func = getCallback("__new");
  _pin_func = getCallback("__pin");
  _unpin_func = getCallback("__unpin");
  _collect_func = getCallback("__collect");

  if (!_new_func || !_pin_func || !_unpin_func || !_collect_func) {
    log_wrn("WebAssembly instance does not export full AssemblyScript runtime");
  }
}

ScriptInstance::~ScriptInstance() {
  if (_module_instance != nullptr) {
    wasm_extern_vec_delete(&_instance_externs);
    wasm_instance_delete(_module_instance);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Callback helpers
////////////////////////////////////////////////////////////////////////////////

void ScriptInstance::addCallback(const types::string& symbol,
                                 wasm_func_t* func) {
  _callbacks.emplace(symbol, func);
}

bool ScriptInstance::hasCallback(const types::string& symbol) {
  return _callbacks.find(symbol) != _callbacks.end();
}

wasm_func_t* ScriptInstance::getCallback(const types::string& symbol) {
  auto iter = _callbacks.find(symbol);

  if (iter != _callbacks.end()) {
    return iter->second;
  } else {
    return nullptr;
  }
}

bool ScriptInstance::runCallback(const types::string& symbol,
                                 const wasm_val_t* args, size_t arg_num,
                                 wasm_val_t* results, size_t result_num) {
  auto iter = _callbacks.find(symbol);

  if (iter == _callbacks.end()) {
    log_err_fmt("Attempted to run missing callback %s", symbol.c_str());
    return false;
  }

  if (runFunction(iter->second, args, arg_num, results, result_num)) {
    return true;
  } else {
    log_err_fmt("Error while running callback %s", symbol.c_str());
    return false;
  }
}

bool ScriptInstance::runFunction(wasm_func_t* func, const wasm_val_t* args,
                                 size_t arg_num, wasm_val_t* results,
                                 size_t result_num) {
  if (func == nullptr) {
    log_err_fmt("Attempted to run null function");
    return false;
  }

  wasmtime_error_t* module_error = nullptr;
  wasm_trap_t* module_trap = nullptr;
  module_error = wasmtime_func_call(func, args, arg_num, results, result_num,
                                    &module_trap);
  if (scripts->handleError(module_error, module_trap)) {
    log_err_fmt("Error while running function");
    return false;
  } else {
    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// AssemblyScript memory management helpers
////////////////////////////////////////////////////////////////////////////////

bool ScriptInstance::AS_new(uint32_t size, uint32_t id, uint32_t* ptr) {
  std::array<wasm_val_t, 2> args;

  auto& size_arg = args[0];
  size_arg.kind = WASM_I32;
  size_arg.of.i32 = size;

  auto& id_arg = args[1];
  id_arg.kind = WASM_I32;
  id_arg.of.i32 = id;

  wasm_val_t ptr_result;

  if (!runFunction(_new_func, args.data(), args.size(), &ptr_result, 1)) {
    log_err("Failed to allocate AssemblyScript object");
    *ptr = 0;
    return false;
  }

  *ptr = ptr_result.of.i32;
  return true;
}

bool ScriptInstance::AS_pin(uint32_t ptr) {
  if (ptr == 0) {
    log_err("Attempted to pin null object");
    return false;
  }

  wasm_val_t ptr_arg;
  ptr_arg.kind = WASM_I32;
  ptr_arg.of.i32 = ptr;

  wasm_val_t ptr_result;  // Unused

  if (!runFunction(_pin_func, &ptr_arg, 1, &ptr_result, 1)) {
    log_err("Failed to pin AssemblyScript object");
    return false;
  } else {
    return true;
  }
}

bool ScriptInstance::AS_unpin(uint32_t ptr) {
  if (ptr == 0) {
    log_err("Attempted to unpin null object");
    return false;
  }

  wasm_val_t ptr_arg;
  ptr_arg.kind = WASM_I32;
  ptr_arg.of.i32 = ptr;

  if (!runFunction(_unpin_func, &ptr_arg, 1, nullptr, 0)) {
    log_err("Failed to unpin AssemblyScript object");
    return false;
  } else {
    return true;
  }
}

bool ScriptInstance::AS_collect() {
  if (!runFunction(_collect_func, nullptr, 0, nullptr, 0)) {
    log_err("Failed to perform AssemblyScript garbage collection");
    return false;
  } else {
    return true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// AssemblyScript object management helpers
////////////////////////////////////////////////////////////////////////////////

bool ScriptInstance::AS_construct(const types::string& object_name,
                                  const wasm_val_t* args, size_t arg_num,
                                  uint32_t* ptr) {
  types::string constructor_name = object_name + "#constructor";

  wasm_func_t* constructor_func = getCallback(constructor_name);
  if (constructor_func == nullptr) {
    log_err_fmt("Object %s does not export constructor", object_name.c_str());
    return false;
  }

  types::vector<wasm_val_t> constructor_args(1 + arg_num);

  auto& this_arg = constructor_args[0];
  this_arg.kind = WASM_I32;
  this_arg.of.i32 = 0;  // Passing 0 to a constructor allocates a new object

  memcpy(&constructor_args[1], args, arg_num * sizeof(wasm_val_t));

  wasm_val_t this_result;

  if (runFunction(constructor_func, constructor_args.data(),
                  constructor_args.size(), &this_result, 1)) {
    *ptr = this_result.of.i32;
    return true;
  } else {
    log_err_fmt("Failed to construct %s", object_name.c_str());
    return false;
  }
}

ASObjectHeader* ScriptInstance::AS_getHeader(uint32_t ptr) {
  if (_memory == nullptr) {
    log_err("Wasm instance does not export memory");
    return nullptr;
  }

  if (ptr < sizeof(ASObjectHeader)) {
    log_err("Object pointer is not valid");
    return nullptr;
  }

  byte_t* memory = wasm_memory_data(_memory);
  byte_t* header_addr = &memory[ptr] - sizeof(ASObjectHeader);
  return reinterpret_cast<ASObjectHeader*>(header_addr);
}

ASObjectHeader* ScriptInstance::AS_assertType(uint32_t ptr, uint32_t id) {
  if (ptr == 0) {
    return nullptr;
  }

  ASObjectHeader* header = AS_getHeader(ptr);

  if (header->rt_id != id) {
    return nullptr;
  } else {
    return header;
  }
}

bool ScriptInstance::AS_getString(uint32_t ptr, types::string* data) {
  if (_memory == nullptr) {
    log_err("Wasm instance does not export memory");
    return false;
  }

  // The type ID of String is 1
  ASObjectHeader* header = AS_assertType(ptr, 1);

  if (header == nullptr) {
    log_err("Object is not a string");
    return false;
  }

  byte_t* memory = wasm_memory_data(_memory);

  // TODO(marceline-cramer) Find better way to convert from UTF-16
  char16_t* utf_begin = reinterpret_cast<char16_t*>(&memory[ptr]);
  char16_t* utf_end = utf_begin + header->rt_size / 2;

  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
  *data = convert.to_bytes(utf_begin, utf_end);

  return true;
}

bool ScriptInstance::AS_newString(const types::string& data, uint32_t* ptr) {
  if (_memory == nullptr) {
    log_err("Wasm instance does not export memory");
    return false;
  }

  uint32_t size = data.length() << 1;

  // The type ID of String is 1
  if (!AS_new(size, 1, ptr)) {
    log_err("Failed to allocate AssemblyScript string");
    return false;
  }

  byte_t* memory = wasm_memory_data(_memory);
  char16_t* utf_begin = reinterpret_cast<char16_t*>(&memory[*ptr]);

  memset(utf_begin, 0, size);

  for (uint32_t i = 0; i < data.length(); i++) {
    utf_begin[i] = data[i];
  }

  return true;
}

}  // namespace core
}  // namespace mondradiko
