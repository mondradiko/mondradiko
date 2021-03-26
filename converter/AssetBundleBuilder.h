// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <thread>
#include <unordered_set>
#include <vector>

#include "lib/include/flatbuffers_headers.h"
#include "types/assets/AssetTypes.h"
#include "types/assets/Registry_generated.h"
#include "types/assets/SerializedAsset_generated.h"

namespace mondradiko {
namespace converter {

class AssetBundleBuilder {
 public:
  explicit AssetBundleBuilder(const std::filesystem::path&);
  ~AssetBundleBuilder();

  void setDefaultCompressionMethod(assets::LumpCompressionMethod method) {
    default_compression = method;
  }

  assets::AssetResult addAsset(assets::AssetId*,
                               flatbuffers::FlatBufferBuilder*,
                               flatbuffers::Offset<assets::SerializedAsset>);
  assets::AssetResult addInitialPrefab(assets::AssetId);
  assets::AssetResult buildBundle(const char*);

 private:
  std::filesystem::path bundle_root;

  assets::LumpCompressionMethod default_compression =
      assets::LumpCompressionMethod::None;

  struct AssetToSave {
    assets::AssetId id;
    size_t size;
  };

  struct LumpToSave {
    size_t total_size;
    char* data;

    std::vector<AssetToSave> assets;

    // Finalized metadata
    std::thread* finalizer_thread;
    std::filesystem::path lump_path;
    assets::LumpCompressionMethod compression_method;
    assets::LumpHash checksum;
    assets::LumpHashMethod hash_method;
  };

  std::vector<LumpToSave*> lumps;
  std::unordered_set<assets::AssetId> used_ids;
  std::vector<assets::AssetId> initial_prefabs;

  void launchFinalizer(LumpToSave*);
  LumpToSave* allocateLump(uint32_t);

  static void finalizeLump(LumpToSave*, assets::LumpCompressionMethod,
                           assets::LumpHashMethod);
};

}  // namespace converter
}  // namespace mondradiko
