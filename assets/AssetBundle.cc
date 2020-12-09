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

    {
      // Check max lumps
    }

    for (uint32_t lump_index = 0; lump_index < header.lump_count;
         lump_index++) {
      AssetRegistryLumpEntry lump_entry;
      registry_file.read(reinterpret_cast<char*>(&lump_entry),
                         sizeof(lump_entry));

      {
          // Check sum :P
      }

      {
        // Check max assets
      }

      for (uint32_t asset_index = 0; asset_index < lump_entry.asset_count;
           asset_index++) {
        AssetRegistryEntry asset_entry;
        registry_file.read(reinterpret_cast<char*>(&asset_entry),
                           sizeof(asset_entry));

        {
          // Add to unordered_map
        }
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
