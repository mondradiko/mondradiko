/**
 * @file wasm_converter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates ScriptAssets from compiled Wasm modules.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "converter/wasm_converter.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "assets/format/ScriptAsset_generated.h"
#include "log/log.h"

namespace mondradiko {

assets::AssetId wat_convert(assets::AssetBundleBuilder* builder,
                            std::filesystem::path script_path) {
  if (!std::filesystem::exists(script_path)) {
    log_ftl("Script %s does not exist", script_path.c_str());
  }

  std::ifstream script_file(script_path);

  script_file.seekg(0, std::ios::end);
  std::streampos length = script_file.tellg();
  script_file.seekg(0, std::ios::beg);

  std::vector<uint8_t> script_data(length);
  script_file.read(reinterpret_cast<char*>(script_data.data()), length);
  script_file.close();

  flatbuffers::FlatBufferBuilder fbb;

  auto data_offset = fbb.CreateVector(script_data);

  assets::ScriptAssetBuilder script_asset(fbb);
  script_asset.add_type(assets::ScriptType::WasmText);
  script_asset.add_data(data_offset);
  auto script_offset = script_asset.Finish();

  assets::SerializedAssetBuilder asset(fbb);
  asset.add_type(assets::AssetType::ScriptAsset);
  asset.add_script(script_offset);
  auto asset_offset = asset.Finish();

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &fbb, asset_offset);
  return asset_id;
}

}  // namespace mondradiko
