/**
 * @file AssetRegistry.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains global asset version, and info into
 * where each asset is stored, by ID.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "assets/Asset.h"
#include "assets/AssetLump.h"

namespace mondradiko {
namespace assets {

struct AssetRegistryHeader {
  char magic[4];
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

class AssetRegistry {
 public:
  AssetRegistry(const char*);
  ~AssetRegistry();

 private:
  std::vector<AssetLump*> lump_cache;
};

}  // namespace assets
}  // namespace mondradiko
