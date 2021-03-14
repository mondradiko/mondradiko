// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bundler/ConverterInterface.h"

namespace mondradiko {

std::filesystem::path ConverterInterface::_getAssetPath(
    std::filesystem::path source_path, const toml::table& config) const {
  const auto& asset_file = config.at("file").as_string();
  auto asset_path = source_path / asset_file.str;
  return asset_path;
}

}  // namespace mondradiko
