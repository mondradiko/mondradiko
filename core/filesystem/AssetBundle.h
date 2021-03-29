// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>

#include "core/filesystem/AssetLump.h"
#include "types/assets/AssetTypes.h"
#include "types/containers/unordered_map.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

class AssetBundle {
 public:
  explicit AssetBundle(const std::filesystem::path&);
  ~AssetBundle();

  assets::AssetResult loadRegistry(const char*);

  void getChecksums(types::vector<assets::LumpHash>&);
  void getInitialPrefabs(types::vector<assets::AssetId>&);
  bool isAssetRegistered(assets::AssetId);
  bool loadAsset(const assets::SerializedAsset**, assets::AssetId);

 private:
  std::filesystem::path bundle_root;

  types::vector<assets::AssetId> initial_prefabs;

  struct AssetLookupEntry {
    uint32_t lump_index;
    uint32_t offset;
    uint32_t size;
  };

  types::unordered_map<assets::AssetId, AssetLookupEntry> asset_lookup;

  struct LumpCacheEntry {
    AssetLump* lump;
    size_t file_size;
    assets::LumpCompressionMethod compression_method;
    assets::LumpHashMethod hash_method;
    assets::LumpHash checksum;
  };

  types::vector<LumpCacheEntry> lump_cache;
};

}  // namespace core
}  // namespace mondradiko
