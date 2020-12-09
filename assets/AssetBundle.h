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
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

#include "assets/Asset.h"
#include "assets/AssetLump.h"

namespace mondradiko {
namespace assets {

const size_t ASSET_REGISTRY_MAGIC_LENGTH = 4;
const char ASSET_REGISTRY_MAGIC[] = "MDOA";

struct AssetRegistryHeader {
  char magic[ASSET_REGISTRY_MAGIC_LENGTH];
  uint32_t version;

  AssetHashMethod hash_method;
  AssetLumpCompressionMethod compression_method;

  uint32_t lump_count;
  uint32_t asset_count;
};

struct AssetRegistryLumpEntry {
  AssetLumpHash checksum;
  uint32_t asset_count;
};

struct AssetRegistryEntry {
  uint32_t size;
};

class AssetBundle {
 public:
  AssetBundle(const std::filesystem::path&);
  ~AssetBundle();

  AssetResult loadRegistry(const char*);

 private:
  std::filesystem::path bundle_root;
  std::vector<AssetLump*> lump_cache;
};

}  // namespace assets
}  // namespace mondradiko
