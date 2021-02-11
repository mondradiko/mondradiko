// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <unordered_set>
#include <vector>

#include "assets/common/AssetTypes.h"
#include "lib/include/flatbuffers_headers.h"
#include "types/assets/Registry_generated.h"
#include "types/assets/SerializedAsset_generated.h"

namespace mondradiko {
namespace assets {

class AssetBundleBuilder {
 public:
  explicit AssetBundleBuilder(const std::filesystem::path&);
  ~AssetBundleBuilder();

  AssetResult addAsset(AssetId*, flatbuffers::FlatBufferBuilder*,
                       flatbuffers::Offset<SerializedAsset>);
  AssetResult addInitialPrefab(AssetId);
  AssetResult buildBundle(const char*);

 private:
  std::filesystem::path bundle_root;

  struct AssetToSave {
    AssetId id;
    size_t size;
  };

  struct LumpToSave {
    LumpCompressionMethod compression_method;
    size_t total_size;
    char* data;

    std::vector<AssetToSave> assets;
  };

  std::vector<LumpToSave> lumps;
  std::unordered_set<AssetId> used_ids;
  std::vector<AssetId> initial_prefabs;

  void allocateLump(LumpToSave*);
  void compressLump(LumpToSave*);
};

}  // namespace assets
}  // namespace mondradiko
