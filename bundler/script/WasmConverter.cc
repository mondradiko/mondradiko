// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bundler/script/WasmConverter.h"

#include <fstream>
#include <string>
#include <vector>

#include "bundler/Bundler.h"
#include "log/log.h"

namespace mondradiko {

// Helper function for WebAssembly errors
bool handleError(wasmtime_error_t* error) {
  wasm_byte_vec_t error_message;
  if (error != nullptr) {
    wasmtime_error_message(error, &error_message);
    wasmtime_error_delete(error);
  } else {
    // Return false if no error was thrown
    return false;
  }

  std::string error_string(error_message.data, error_message.size);
  wasm_byte_vec_delete(&error_message);
  log_err_fmt("Wasmtime error thrown: %s", error_string.c_str());
  return true;
}

WasmConverter::WasmConverter(Bundler* bundler) : _bundler(bundler) {
  // Create the engine
  engine = wasm_engine_new();
  if (engine == nullptr) {
    log_ftl("Failed to create Wasm engine");
  }

  // Create the store
  store = wasm_store_new(engine);
  if (store == nullptr) {
    log_ftl("Failed to create Wasm store");
  }
}

WasmConverter::~WasmConverter() {
  if (store) wasm_store_delete(store);
  if (engine) wasm_engine_delete(engine);
}

WasmConverter::AssetOffset WasmConverter::convert(
    AssetBuilder* fbb, std::filesystem::path source_path,
    const toml::table& config) const {
  auto wasm_path = _getAssetPath(source_path, config);
  std::ifstream script_file(wasm_path);

  if (!script_file.is_open()) {
    log_ftl_fmt("Failed to load %s", wasm_path.c_str());
  }

  script_file.seekg(0, std::ios::end);
  std::streampos file_length = script_file.tellg();
  script_file.seekg(0, std::ios::beg);

  std::vector<uint8_t> script_data(file_length);
  script_file.read(reinterpret_cast<char*>(script_data.data()), file_length);
  script_file.close();

  auto data_offset = fbb->CreateVector(script_data.data(), file_length);

  // Load classes from environment
  std::vector<flatbuffers::Offset<assets::ScriptClass>> class_list;
  auto environment_iter = config.find("environment");
  if (environment_iter == config.end()) {
    log_msg("Script has no environment");
  } else {
    std::string script_environment = config.at("environment").as_string();

    if (script_environment != "AssemblyScript") {
      log_ftl_fmt("Unrecognized environment %s", script_environment.c_str());
    }

    log_msg_fmt("Loading from %s environment", script_environment.c_str());

    wasm_module_t* script_module;

    {  // Compile wat
      wasm_byte_vec_t module_data;

      {  // Load wat data
        wasm_byte_vec_new(
            &module_data, script_data.size(),
            reinterpret_cast<const wasm_byte_t*>(script_data.data()));
      }

      wasm_byte_vec_t translated_data;

      {  // Convert text WebAssembly to binary WebAssembly
        wasmtime_error_t* module_error =
            wasmtime_wat2wasm(&module_data, &translated_data);
        if (handleError(module_error)) {
          log_ftl("Failed to convert WebAssembly module");
        }
      }

      {  // Compile the module
        wasmtime_error_t* module_error =
            wasmtime_module_new(engine, &translated_data, &script_module);
        if (handleError(module_error)) {
          log_ftl("Failed to compile WebAssembly module");
        }
      }

      {  // Clean up
        wasm_byte_vec_delete(&module_data);
        wasm_byte_vec_delete(&translated_data);
      }
    }

    std::vector<std::string> exported_functions;

    {  // Scrape exported functions
      wasm_exporttype_vec_t export_types;
      wasm_module_exports(script_module, &export_types);

      for (uint32_t i = 0; i < export_types.size; i++) {
        wasm_exporttype_t* export_type = export_types.data[i];

        const wasm_externtype_t* extern_type =
            wasm_exporttype_type(export_type);
        wasm_externkind_t extern_kind = wasm_externtype_kind(extern_type);
        if (extern_kind == WASM_EXTERN_FUNC) {
          const wasm_name_t* export_name = wasm_exporttype_name(export_type);
          auto export_string =
              std::string(export_name->data, export_name->size);
          exported_functions.push_back(export_string);
        }
      }

      wasm_exporttype_vec_delete(&export_types);
    }

    auto classes_iter = config.find("classes");
    if (classes_iter != config.end() && classes_iter->second.is_array()) {
      const auto& classes = classes_iter->second.as_array();
      for (auto& wasm_class : classes) {
        std::string class_name = wasm_class.at("name").as_string();

        log_msg_fmt("Importing class %s", class_name.c_str());

        std::vector<flatbuffers::Offset<assets::ScriptMethod>> method_list;

        for (auto method_name : exported_functions) {
          std::string event;
          std::string symbol;

          // Test if method_name starts with the class name + #
          if (method_name.rfind(class_name + "#", 0) != 0) {
            // Discard methods from other classes
            continue;
          }

          size_t sym_offset = class_name.size() + 1;
          if (method_name.rfind("get:", sym_offset) == sym_offset) {
            // Discard getters
            continue;
          }

          if (method_name.rfind("set:", sym_offset) == sym_offset) {
            // Discard setters
            continue;
          }

          if (method_name.substr(sym_offset) == "constructor") {
            // Discard the constructor
            continue;
          }

          event = method_name.substr(sym_offset);
          symbol = method_name;

          log_dbg_fmt("Adding %s() @ %s", event.c_str(), symbol.c_str());

          auto event_offset = fbb->CreateString(event);
          auto symbol_offset = fbb->CreateString(symbol);

          assets::ScriptMethodBuilder method_builder(*fbb);
          method_builder.add_event(event_offset);
          method_builder.add_symbol(symbol_offset);

          auto method_offset = method_builder.Finish();
          method_list.push_back(method_offset);
        }

        auto methods_offset = fbb->CreateVector(method_list);

        {
          std::string constructor_symbol = class_name + "#constructor";
          auto constructor_offset = fbb->CreateString(constructor_symbol);

          assets::AssemblyScriptClassBuilder assemblyscript_class(*fbb);
          assemblyscript_class.add_constructor_symbol(constructor_offset);
          assemblyscript_class.add_methods(methods_offset);
          auto assemblyscript_class_offset = assemblyscript_class.Finish();

          assets::ScriptClassBuilder class_builder(*fbb);
          class_builder.add_assemblyscript(assemblyscript_class_offset);
          auto class_offset = class_builder.Finish();
          class_list.push_back(class_offset);
        }
      }
    }

    wasm_module_delete(script_module);
  }

  auto classes_offset = fbb->CreateVector(class_list);

  assets::ScriptAssetBuilder script_asset(*fbb);
  // TODO(marceline-cramer) Binary Wasm scripts
  script_asset.add_type(assets::ScriptType::WasmText);
  script_asset.add_data(data_offset);
  script_asset.add_classes(classes_offset);
  auto script_offset = script_asset.Finish();

  assets::SerializedAssetBuilder asset(*fbb);
  asset.add_type(assets::AssetType::ScriptAsset);
  asset.add_script(script_offset);
  auto asset_offset = asset.Finish();

  return asset_offset;
}  // namespace mondradiko

}  // namespace mondradiko
