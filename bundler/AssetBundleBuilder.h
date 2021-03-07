// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <thread>
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

  void setDefaultCompressionMethod(LumpCompressionMethod method) {
    default_compression = method;
  }

  AssetResult addAsset(AssetId*, flatbuffers::FlatBufferBuilder*,
                       flatbuffers::Offset<SerializedAsset>);
  AssetResult addInitialPrefab(AssetId);
  AssetResult buildBundle(const char*);

 private:
  std::filesystem::path bundle_root;

  LumpCompressionMethod default_compression = LumpCompressionMethod::None;

  struct AssetToSave {
    AssetId id;
    size_t size;
  };

  struct LumpToSave {
    size_t total_size;
    char* data;

    std::vector<AssetToSave> assets;

    // Finalized metadata
    std::thread* finalizer_thread;
    std::filesystem::path lump_path;
    LumpCompressionMethod compression_method;
    LumpHash checksum;
    LumpHashMethod hash_method;
  };

  std::vector<LumpToSave*> lumps;
  std::unordered_set<AssetId> used_ids;
  std::vector<AssetId> initial_prefabs;

  void launchFinalizer(LumpToSave*);
  LumpToSave* allocateLump(uint32_t);

  static void finalizeLump(LumpToSave*, LumpCompressionMethod, LumpHashMethod);
};

}  // namespace assets
}  // namespace mondradiko
