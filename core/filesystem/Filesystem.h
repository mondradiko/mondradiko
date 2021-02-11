// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <vector>

#include "core/assets/Asset.h"
#include "core/filesystem/AssetBundle.h"
#include "lib/include/toml_headers.h"

namespace mondradiko {

class Filesystem {
 public:
  Filesystem();
  ~Filesystem();

  bool loadAssetBundle(const std::filesystem::path&);
  void getChecksums(std::vector<assets::LumpHash>&);
  void getInitialPrefabs(std::vector<assets::AssetId>&);
  bool loadAsset(const assets::SerializedAsset**, AssetId);

  toml::value loadToml(const std::filesystem::path&);

 private:
  std::vector<assets::AssetBundle*> asset_bundles;
};

}  // namespace mondradiko
