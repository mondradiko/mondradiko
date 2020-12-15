/**
 * @file AssetBundle.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Loads an asset bundle, its registry, its
 * lumps, and the assets themselves.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "assets/format/AssetTypes.h"
#include "assets/loading/AssetLump.h"
#include "assets/loading/ImmutableAsset.h"

namespace mondradiko {
namespace assets {

class AssetBundle {
 public:
  explicit AssetBundle(const std::filesystem::path&);
  ~AssetBundle();

  AssetResult loadRegistry(const char*);

  bool isAssetRegistered(AssetId);
  bool loadAsset(ImmutableAsset*, AssetId);

 private:
  std::filesystem::path bundle_root;

  struct AssetLookupEntry {
    uint32_t lump_index;
    uint32_t offset;
    uint32_t size;
  };

  std::unordered_map<AssetId, AssetLookupEntry> asset_lookup;
  std::vector<AssetLump*> lump_cache;
};

}  // namespace assets
}  // namespace mondradiko
