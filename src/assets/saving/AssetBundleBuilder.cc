/**
 * @file AssetBundleBuilder.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Allocates, sorts, compresses, and saves an
 * AssetBundle and its assets to disk.
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/saving/AssetBundleBuilder.h"

#include <cstring>
#include <fstream>

namespace mondradiko {
namespace assets {

AssetBundleBuilder::AssetBundleBuilder(const std::filesystem::path& bundle_root)
    : bundle_root(bundle_root) {}

AssetBundleBuilder::~AssetBundleBuilder() {
  for (auto& lump : lumps) {
    delete[] lump.data;
  }
}

AssetResult AssetBundleBuilder::addAsset(AssetId id, const void* data,
                                         size_t size) {
  if (lumps.size() == 0) {
    lumps.resize(1);
    allocateLump(&lumps[0]);
  }

  uint32_t lump_index = lumps.size() - 1;

  if (lumps[lump_index].total_size + size > ASSET_LUMP_MAX_SIZE) {
    LumpToSave new_lump;
    allocateLump(&new_lump);
    lumps.push_back(new_lump);
    lump_index++;
  }

  AssetToSave new_asset;
  new_asset.id = id;
  new_asset.size = size;
  lumps[lump_index].assets.push_back(new_asset);

  memcpy(lumps[lump_index].data + lumps[lump_index].total_size, data, size);

  lumps[lump_index].total_size += size;

  return AssetResult::Success;
}

AssetResult AssetBundleBuilder::buildBundle(const char* registry_name) {
  for (uint32_t lump_index = 0; lump_index < lumps.size(); lump_index++) {
    auto& lump = lumps[lump_index];
    auto lump_name = generateLumpName(lump_index);
    auto lump_path = bundle_root / lump_name;
    std::ofstream lump_file(lump_path.c_str(), std::ofstream::binary);

    // TODO(marceline-cramer) Lump compression

    lump_file.write(reinterpret_cast<char*>(lump.data), lump.total_size);

    lump_file.close();
  }

  auto registry_path = bundle_root / registry_name;
  std::ofstream registry_file(registry_path.c_str());

  {
    AssetRegistryHeader header;
    strncpy(header.magic, ASSET_REGISTRY_MAGIC, ASSET_REGISTRY_MAGIC_LENGTH);
    header.version = MONDRADIKO_ASSET_VERSION;
    header.lump_count = lumps.size();
    registry_file.write(reinterpret_cast<char*>(&header), sizeof(header));
  }

  for (uint32_t lump_index = 0; lump_index < lumps.size(); lump_index++) {
    auto& lump = lumps[lump_index];

    {
      AssetRegistryLumpEntry lump_entry;
      lump_entry.checksum = 0;  // TODO(marceline-cramer) Generate checksums
      lump_entry.hash_method = LumpHashMethod::None;
      lump_entry.compression_method = LumpCompressionMethod::None;
      lump_entry.asset_count = lump.assets.size();
      registry_file.write(reinterpret_cast<char*>(&lump_entry),
                          sizeof(lump_entry));
    }

    for (auto& asset : lump.assets) {
      AssetRegistryEntry entry;
      entry.id = asset.id;
      entry.size = asset.size;
      registry_file.write(reinterpret_cast<char*>(&entry), sizeof(entry));
    }
  }

  registry_file.close();

  return AssetResult::Success;
}

void AssetBundleBuilder::allocateLump(LumpToSave* new_lump) {
  new_lump->total_size = 0;
  new_lump->data = new char[ASSET_LUMP_MAX_SIZE];
  new_lump->assets.resize(0);
}

}  // namespace assets
}  // namespace mondradiko
