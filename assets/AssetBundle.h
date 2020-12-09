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
#include <vector>

#include "assets/Asset.h"
#include "assets/AssetLump.h"

namespace mondradiko {
namespace assets {

const size_t ASSET_REGISTRY_MAGIC_LENGTH = 4;
const char ASSET_REGISTRY_MAGIC[] = "MDOA";

const uint32_t ASSET_REGISTRY_MAX_LUMPS = 256;
const uint32_t ASSET_LUMP_MAX_ASSETS = 4096;
const uint32_t ASSET_LUMP_MAX_SIZE = 128 * 1024 * 1024;  // 128 MiB

struct AssetRegistryHeader {
  char magic[ASSET_REGISTRY_MAGIC_LENGTH];
  uint32_t version;

  uint32_t lump_count;
};

struct AssetRegistryLumpEntry {
  LumpHash checksum;
  LumpHashMethod hash_method;
  LumpCompressionMethod compression_method;
  uint32_t asset_count;
};

struct AssetRegistryEntry {
  AssetId id;
  uint32_t size;
};

class AssetBundle {
 public:
  explicit AssetBundle(const std::filesystem::path&);
  ~AssetBundle();

  AssetResult loadRegistry(const char*);

 private:
  std::filesystem::path bundle_root;
  std::vector<AssetLump*> lump_cache;
};

}  // namespace assets
}  // namespace mondradiko
