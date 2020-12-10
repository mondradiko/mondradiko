/**
 * @file AssetBundleBuilder.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Allocates, sorts, compresses, and saves an
 * AssetBundle and its assets to disk.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "assets/AssetFile.h"

namespace mondradiko {
namespace assets {

class AssetBundleBuilder {
 public:
  AssetBundleBuilder(const std::filesystem::path&);
  ~AssetBundleBuilder();

  AssetResult addAsset(AssetId, const void*, size_t);
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
  void allocateLump(LumpToSave*);
};

}  // namespace assets
}  // namespace mondradiko
