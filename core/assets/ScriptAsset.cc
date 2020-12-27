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

#include "assets/format/ScriptAsset.h"
#include "core/scripting/ScriptEnvironment.h"

namespace mondradiko {

ScriptAsset::ScriptAsset(assets::ImmutableAsset& asset, AssetPool*,
                         ScriptEnvironment* scripts)
    : scripts(scripts) {
  assets::ScriptHeader header;
  asset >> header;

  if (header.type == assets::ScriptType::None) {
    log_err("Trying to load a null script");
    return;
  }

  // TODO(marceline-cramer) wasmtime error checking

  size_t script_size;
  const char* script_data = asset.getData(&script_size);
  wasm_byte_vec_t module_data;
  wasm_byte_vec_new(&module_data, script_size, script_data);

  if (header.type == assets::ScriptType::Binary) {
    script_module = wasm_module_new(scripts->getStore(), &module_data);
  } else if (header.type == assets::ScriptType::Text) {
    wasm_byte_vec_t translated_data;
    wasmtime_wat2wasm(&module_data, &translated_data);
    script_module = wasm_module_new(scripts->getStore(), &translated_data);
    wasm_byte_vec_delete(&translated_data);
  } else {
    log_ftl("Unrecognized ScriptAsset type %d", header.type);
  }

  wasm_byte_vec_delete(&module_data);
}

ScriptAsset::~ScriptAsset() {
  if (script_module) wasm_module_delete(script_module);
}

}  // namespace mondradiko
