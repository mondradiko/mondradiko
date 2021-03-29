// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/script/WasmConverter.h"

#include <fstream>
#include <vector>

namespace mondradiko {
namespace converter {

WasmConverter::AssetOffset WasmConverter::convert(
    AssetBuilder* fbb, std::filesystem::path wasm_path) const {
  std::ifstream script_file(wasm_path);

  script_file.seekg(0, std::ios::end);
  std::streampos length = script_file.tellg();
  script_file.seekg(0, std::ios::beg);

  std::vector<uint8_t> script_data(length);
  script_file.read(reinterpret_cast<char*>(script_data.data()), length);
  script_file.close();

  auto data_offset = fbb->CreateVector(script_data);

  assets::ScriptAssetBuilder script_asset(*fbb);
  // TODO(marceline-cramer) Binary Wasm scripts
  script_asset.add_type(assets::ScriptType::WasmText);
  script_asset.add_data(data_offset);
  auto script_offset = script_asset.Finish();

  assets::SerializedAssetBuilder asset(*fbb);
  asset.add_type(assets::AssetType::ScriptAsset);
  asset.add_script(script_offset);
  auto asset_offset = asset.Finish();

  return asset_offset;
}

}  // namespace converter
}  // namespace mondradiko
