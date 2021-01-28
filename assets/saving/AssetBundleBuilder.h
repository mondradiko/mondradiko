// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <unordered_set>
#include <vector>

#include "assets/common/AssetTypes.h"
#include "assets/format/Registry_generated.h"
#include "assets/format/SerializedAsset_generated.h"
#include "flatbuffers/flatbuffers.h"

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
    size_t total_size;
    char* data;

    std::vector<AssetToSave> assets;
  };

  std::vector<LumpToSave> lumps;
  std::unordered_set<AssetId> used_ids;
  std::vector<AssetId> initial_prefabs;

  void allocateLump(LumpToSave*);
};

}  // namespace assets
}  // namespace mondradiko
