/**
 * @file Filesystem.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to the read-only files in an asset archive.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <filesystem>
#include <vector>

#include "assets/loading/AssetBundle.h"
#include "core/assets/Asset.h"

namespace mondradiko {

class Filesystem {
 public:
  Filesystem();
  ~Filesystem();

  bool loadAssetBundle(const std::filesystem::path&);
  void getChecksums(std::vector<assets::LumpHash>&);
  void getInitialPrefabs(std::vector<assets::AssetId>&);
  bool loadAsset(const assets::SerializedAsset**, AssetId);

 private:
  std::vector<assets::AssetBundle*> asset_bundles;
};

}  // namespace mondradiko
