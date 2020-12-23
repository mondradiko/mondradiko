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

#include "assets/loading/AssetBundle.h"

#include <cstring>
#include <fstream>

#include "assets/format/AssetFile.h"
#include "log/log.h"

namespace mondradiko {
namespace assets {

AssetBundle::AssetBundle(const std::filesystem::path& bundle_root)
    : bundle_root(bundle_root) {}

AssetBundle::~AssetBundle() {
  for (const auto& cached_lump : lump_cache) {
    if (cached_lump.lump) delete cached_lump.lump;
  }
}

AssetResult AssetBundle::loadRegistry(const char* registry_name) {
  auto registry_path = bundle_root / registry_name;

  log_dbg("Opening asset bundle at %s", registry_path.c_str());

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
      log_err("Asset registry lump count exceeds limit of %d",
              ASSET_REGISTRY_MAX_LUMPS);
      return AssetResult::BadSize;
    }

    lump_cache.resize(header.lump_count, {.lump = nullptr});

    for (uint32_t lump_index = 0; lump_index < header.lump_count;
         lump_index++) {
      AssetRegistryLumpEntry lump_entry;
      registry_file.read(reinterpret_cast<char*>(&lump_entry),
                         sizeof(lump_entry));

      if (lump_entry.asset_count > ASSET_LUMP_MAX_ASSETS) {
        registry_file.close();
        log_err("Asset lump asset count exceeds limit of %d",
                ASSET_LUMP_MAX_ASSETS);
        return AssetResult::BadSize;
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

        AssetLookupEntry lookup_entry{.lump_index = lump_index,
                                      .offset = asset_offset,
                                      .size = asset_entry.size};

        asset_lookup.emplace(asset_entry.id, lookup_entry);

        asset_offset += asset_entry.size;
      }

      // asset_offset now represents the size of the whole lump
      if (asset_offset > ASSET_LUMP_MAX_SIZE) {
        registry_file.close();
        return AssetResult::BadSize;
      }

      lump_cache[lump_index].lump = nullptr;
      lump_cache[lump_index].expected_length = asset_offset;
      lump_cache[lump_index].hash_method = lump_entry.hash_method;
      lump_cache[lump_index].checksum = lump_entry.checksum;
      lump_cache[lump_index].compression_method = lump_entry.compression_method;
    }
  } catch (std::ifstream::failure& e) {
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

  for (uint32_t i = 0; i < lump_cache.size(); i++) {
    auto lump_file = generateLumpName(i);
    auto lump_path = bundle_root / lump_file;

    if (!std::filesystem::exists(lump_path)) {
      registry_file.close();
      return AssetResult::FileNotFound;
    }

    AssetLump lump(lump_path);

    if (!lump.assertLength(lump_cache[i].expected_length)) {
      return AssetResult::BadSize;
    }

    if (!lump.assertHash(lump_cache[i].hash_method, lump_cache[i].checksum)) {
      return AssetResult::InvalidChecksum;
    }
  }

  return AssetResult::Success;
}

void AssetBundle::getChecksums(std::vector<LumpHash>& checksums) {
  checksums.resize(lump_cache.size());

  for (uint32_t i = 0; i < lump_cache.size(); i++) {
    checksums[i] = lump_cache[i].checksum;
  }
}

bool AssetBundle::isAssetRegistered(AssetId id) {
  return asset_lookup.find(id) != asset_lookup.end();
}

bool AssetBundle::loadAsset(ImmutableAsset* asset, AssetId id) {
  // TODO(marceline-cramer) Better error checking and logging
  // TODO(marceline-cramer) Check lump checksums when loading into cache
  auto stored_asset = asset_lookup.find(id)->second;
  auto lump_index = stored_asset.lump_index;

  auto& cached_lump = lump_cache[lump_index];

  if (cached_lump.lump == nullptr) {
    cached_lump.lump =
        new AssetLump(bundle_root / generateLumpName(lump_index));

    if (!cached_lump.lump->assertLength(cached_lump.expected_length)) {
      return false;
    }

    if (!cached_lump.lump->assertHash(cached_lump.hash_method,
                                      cached_lump.checksum)) {
      return false;
    }

    cached_lump.lump->decompress(cached_lump.compression_method);
  }

  return cached_lump.lump->loadAsset(asset, stored_asset.offset,
                                     stored_asset.size);
}

}  // namespace assets
}  // namespace mondradiko
