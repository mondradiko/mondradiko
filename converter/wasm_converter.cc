/**
 * @file wasm_converter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates ScriptAssets from compiled Wasm modules.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "converter/wasm_converter.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "assets/format/ScriptAsset.h"
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

  std::vector<char> script_data(length);
  script_file.read(script_data.data(), length);
  script_file.close();

  assets::MutableAsset asset;

  assets::ScriptHeader header;
  header.type = assets::ScriptType::Text;
  header.module_size = script_data.size();
  asset << header;

  asset.writeData(script_data.data(), script_data.size());

  assets::AssetId asset_id;
  builder->addAsset(&asset_id, &asset);
  return asset_id;
}

}  // namespace mondradiko
