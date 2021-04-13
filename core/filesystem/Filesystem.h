// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>

#include "core/assets/Asset.h"
#include "core/filesystem/AssetBundle.h"
#include "lib/include/toml_headers.h"
#include "types/containers/string.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

class Filesystem {
 public:
  Filesystem();
  ~Filesystem();

  bool loadAssetBundle(const std::filesystem::path&);
  void getChecksums(types::vector<assets::LumpHash>&);
  void getInitialPrefabs(types::vector<assets::AssetId>&);
  bool loadAsset(const assets::SerializedAsset**, AssetId);

  toml::value loadToml(const std::filesystem::path&);
  bool loadTextFile(const std::filesystem::path&, types::string*);
  bool loadBinaryFile(const std::filesystem::path&, types::vector<char>*);

 private:
  types::vector<AssetBundle*> asset_bundles;
};

}  // namespace core
}  // namespace mondradiko
