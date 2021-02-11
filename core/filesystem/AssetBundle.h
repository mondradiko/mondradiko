// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "assets/common/AssetTypes.h"
#include "core/filesystem/AssetLump.h"

namespace mondradiko {
namespace assets {

class AssetBundle {
 public:
  explicit AssetBundle(const std::filesystem::path&);
  ~AssetBundle();

  AssetResult loadRegistry(const char*);

  void getChecksums(std::vector<LumpHash>&);
  void getInitialPrefabs(std::vector<AssetId>&);
  bool isAssetRegistered(AssetId);
  bool loadAsset(const SerializedAsset**, AssetId);

 private:
  std::filesystem::path bundle_root;

  std::vector<AssetId> initial_prefabs;

  struct AssetLookupEntry {
    uint32_t lump_index;
    uint32_t offset;
    uint32_t size;
  };

  std::unordered_map<AssetId, AssetLookupEntry> asset_lookup;

  struct LumpCacheEntry {
    AssetLump* lump;
    size_t file_size;
    LumpCompressionMethod compression_method;
    LumpHashMethod hash_method;
    LumpHash checksum;
  };

  std::vector<LumpCacheEntry> lump_cache;
};

}  // namespace assets
}  // namespace mondradiko
