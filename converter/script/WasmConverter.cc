// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/script/WasmConverter.h"

#include <fstream>
#include <vector>

#include "converter/BundlerInterface.h"
#include "log/log.h"

namespace mondradiko {
namespace converter {

WasmConverter::AssetOffset WasmConverter::convert(
    AssetBuilder* fbb, const toml::table& asset) const {
  auto wasm_path = _bundler->getAssetPath(asset);

  std::ifstream script_file(wasm_path, std::ifstream::binary);

  if (!script_file) {
    log_ftl_fmt("Failed to open %s", wasm_path.c_str());
  }

  script_file.seekg(0, std::ios::end);
  std::streampos length = script_file.tellg();
  script_file.seekg(0, std::ios::beg);

  std::vector<uint8_t> script_data(length);
  script_file.read(reinterpret_cast<char*>(script_data.data()), length);
  script_file.close();

  auto data_offset = fbb->CreateVector(script_data);

  assets::ScriptAssetBuilder script_asset(*fbb);
  script_asset.add_type(_script_type);
  script_asset.add_data(data_offset);
  auto script_offset = script_asset.Finish();

  assets::SerializedAssetBuilder serialized_asset(*fbb);
  serialized_asset.add_type(assets::AssetType::ScriptAsset);
  serialized_asset.add_script(script_offset);
  auto asset_offset = serialized_asset.Finish();

  return asset_offset;
}

}  // namespace converter
}  // namespace mondradiko
