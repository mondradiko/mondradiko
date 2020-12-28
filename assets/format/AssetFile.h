/**
 * @file AssetFile.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Structs and constants for asset file manipulation.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/format/AssetTypes.h"

namespace mondradiko {
namespace assets {

static const char ASSET_REGISTRY_MAGIC[] = {'M', 'D', 'O', 'A'};
static const size_t ASSET_REGISTRY_MAGIC_LENGTH = sizeof(ASSET_REGISTRY_MAGIC);

static const uint32_t ASSET_REGISTRY_MAX_LUMPS = 256;
static const uint32_t ASSET_LUMP_MAX_ASSETS = 4096;
static const uint32_t ASSET_LUMP_MAX_SIZE = 128 * 1024 * 1024;  // 128 MiB

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

}  // namespace assets
}  // namespace mondradiko
