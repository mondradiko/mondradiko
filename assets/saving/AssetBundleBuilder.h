/**
 * @file AssetBundleBuilder.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Allocates, sorts, compresses, and saves an
 * AssetBundle and its assets to disk.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <filesystem>
#include <unordered_set>
#include <vector>

#include "assets/format/AssetFile.h"
#include "assets/saving/MutableAsset.h"

namespace mondradiko {
namespace assets {

class AssetBundleBuilder {
 public:
  explicit AssetBundleBuilder(const std::filesystem::path&);
  ~AssetBundleBuilder();

  AssetResult addAsset(AssetId*, MutableAsset*);
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

  void allocateLump(LumpToSave*);
};

}  // namespace assets
}  // namespace mondradiko
