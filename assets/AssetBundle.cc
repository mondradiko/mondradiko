/**
 * @file AssetBundle.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Loads an asset bundle, its registry, its
 * lumps, and the assets themselves.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/AssetBundle.h"

namespace mondradiko {
namespace assets {

AssetBundle::AssetBundle(const std::filesystem::path& bundle_root)
    : bundle_root(bundle_root) {}

AssetBundle::~AssetBundle() {
  for (const auto& lump : lump_cache) {
    if (lump) delete lump;
  }
}

AssetResult AssetBundle::loadRegistry(const char* registry_name) {
  auto registry_path = bundle_root / registry_name;

  if (!std::filesystem::exists(registry_path)) {
    return AssetResult::FileNotFound;
  }

  std::ifstream registry_file(registry_path.c_str(),
                              std::ifstream::in | std::ifstream::binary);
  registry_file.exceptions(std::ifstream::badbit | std::ifstream::eofbit);

  try {
    AssetRegistryHeader header;
    registry_file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (strncmp(header.magic, ASSET_REGISTRY_MAGIC,
                ASSET_REGISTRY_MAGIC_LENGTH) != 0) {
      registry_file.close();
      return AssetResult::WrongMagic;
    }

    if (header.version != MONDRADIKO_ASSET_VERSION) {
      registry_file.close();
      return AssetResult::WrongVersion;
    }

    if (header.lump_count > ASSET_REGISTRY_MAX_LUMPS) {
      registry_file.close();
      return AssetResult::BadSize;
    }

    for (uint32_t lump_index = 0; lump_index < header.lump_count;
         lump_index++) {
      AssetRegistryLumpEntry lump_entry;
      registry_file.read(reinterpret_cast<char*>(&lump_entry),
                         sizeof(lump_entry));

      if (lump_entry.asset_count > ASSET_LUMP_MAX_ASSETS) {
        registry_file.close();
        return AssetResult::BadSize;
      }

      char lump_file[32];
      snprintf(lump_file, sizeof(lump_file), "lump_%04u.bin", lump_index);
      auto lump_path = bundle_root / lump_file;

      if (!std::filesystem::exists(lump_path)) {
        registry_file.close();
        return AssetResult::FileNotFound;
      }

      uint32_t asset_offset = 0;

      for (uint32_t asset_index = 0; asset_index < lump_entry.asset_count;
           asset_index++) {
        AssetRegistryEntry asset_entry;
        registry_file.read(reinterpret_cast<char*>(&asset_entry),
                           sizeof(asset_entry));
        
        auto lookup_iter = asset_lookup.find(asset_entry.id);

        if (lookup_iter != asset_lookup.end()) {
          registry_file.close();
          return AssetResult::DuplicateAsset;
        }

        AssetLookupEntry lookup_entry{
          .lump_index = lump_index,
          .offset = asset_offset,
          .size = asset_entry.size
        };

        asset_lookup.emplace(asset_entry.id, lookup_entry);

        asset_offset += asset_entry.size;
      }

      // asset_offset now represents the size of the whole lump
      if (asset_offset > ASSET_LUMP_MAX_SIZE) {
        registry_file.close();
        return AssetResult::BadSize;
      }

      AssetLump lump(lump_path);

      if (!lump.assertLength(asset_offset)) {
        registry_file.close();
        return AssetResult::BadSize;
      }

      if (!lump.assertHash(lump_entry.hash_method, lump_entry.checksum)) {
        registry_file.close();
        return AssetResult::InvalidChecksum;
      }
    }
  } catch (std::ifstream::failure e) {
    AssetResult result;
    if (registry_file.eof()) {
      result = AssetResult::UnexpectedEof;
    } else {
      result = AssetResult::BadFile;
    }

    registry_file.close();
    return result;
  }

  registry_file.close();
  return AssetResult::Success;
}

}  // namespace assets
}  // namespace mondradiko
