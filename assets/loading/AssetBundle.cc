// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "assets/loading/AssetBundle.h"

#include <cstring>
#include <fstream>

#include "log/log.h"
#include "types/assets/Registry_generated.h"

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
  std::vector<char> registry_data;
  const Registry* registry = nullptr;

  {
    log_zone_named("Load and validate registry file");

    auto registry_path = bundle_root / registry_name;

    log_dbg("Opening asset bundle at %s", registry_path.c_str());

    if (!std::filesystem::exists(registry_path)) {
      return AssetResult::FileNotFound;
    }

    std::ifstream registry_file(registry_path.c_str(), std::ifstream::binary);

    registry_file.seekg(0, std::ios::end);
    std::streampos length = registry_file.tellg();
    registry_file.seekg(0, std::ios::beg);

    registry_data.resize(length);
    registry_file.read(registry_data.data(), length);
    registry_file.close();

    flatbuffers::Verifier verifier(
        reinterpret_cast<const uint8_t*>(registry_data.data()),
        registry_data.size());
    if (!VerifyRegistryBuffer(verifier)) {
      log_err("Asset validation failed");
      return AssetResult::InvalidBuffer;
    }

    registry = GetRegistry(registry_data.data());
  }

  {
    log_zone_named("Check registry metadata");

    if (registry->version() != MONDRADIKO_ASSET_VERSION) {
      return AssetResult::WrongVersion;
    }

    if (registry->lumps() == nullptr) {
      log_err("Missing lump reference in registry");
      return AssetResult::BadContents;
    }

    if (registry->lumps()->size() > ASSET_REGISTRY_MAX_LUMPS) {
      log_err("Registry lump count exceeds limit of %d", ASSET_LUMP_MAX_ASSETS);
      return AssetResult::BadSize;
    }
  }

  {
    log_zone_named("Load initial prefabs");

    initial_prefabs.resize(0);

    for (uint32_t i = 0; i < registry->initial_prefabs()->size(); i++) {
      initial_prefabs.push_back(
          static_cast<AssetId>(registry->initial_prefabs()->Get(i)));
    }
  }

  {
    log_zone_named("Load lumps");

    uint32_t lump_count = registry->lumps()->size();
    lump_cache.resize(lump_count, {.lump = nullptr});

    for (uint32_t lump_index = 0; lump_index < lump_count; lump_index++) {
      const LumpEntry* lump_entry = registry->lumps()->Get(lump_index);

      uint32_t asset_count = lump_entry->assets()->size();

      if (asset_count > ASSET_LUMP_MAX_ASSETS) {
        log_err("Asset lump asset count exceeds limit of %d",
                ASSET_LUMP_MAX_ASSETS);
        return AssetResult::BadSize;
      }

      uint32_t asset_offset = 0;

      for (uint32_t asset_index = 0; asset_index < asset_count; asset_index++) {
        const AssetEntry* asset_entry = lump_entry->assets()->Get(asset_index);

        auto lookup_iter = asset_lookup.find(asset_entry->id());

        if (lookup_iter != asset_lookup.end()) {
          return AssetResult::DuplicateAsset;
        }

        AssetLookupEntry lookup_entry{.lump_index = lump_index,
                                      .offset = asset_offset,
                                      .size = asset_entry->size()};

        asset_lookup.emplace(asset_entry->id(), lookup_entry);

        asset_offset += asset_entry->size();
      }

      // asset_offset now represents the size of the whole lump
      if (asset_offset > ASSET_LUMP_MAX_SIZE) {
        return AssetResult::BadSize;
      }

      lump_cache[lump_index].lump = nullptr;
      lump_cache[lump_index].file_size = lump_entry->file_size();
      lump_cache[lump_index].hash_method = lump_entry->hash_method();
      lump_cache[lump_index].checksum = lump_entry->checksum();
      lump_cache[lump_index].compression_method =
          lump_entry->compression_method();
    }
  }

  {
    log_zone_named("Build and validate lump cache");

    for (uint32_t i = 0; i < lump_cache.size(); i++) {
      auto lump_file = generateLumpName(i);
      auto lump_path = bundle_root / lump_file;

      if (!std::filesystem::exists(lump_path)) {
        return AssetResult::FileNotFound;
      }

      auto& cached_lump = lump_cache[i];
      AssetLump lump(lump_path);

      if (!lump.assertFileSize(cached_lump.file_size)) {
        return AssetResult::BadSize;
      }

      if (!lump.assertHash(cached_lump.hash_method, cached_lump.checksum)) {
        return AssetResult::InvalidChecksum;
      }
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

void AssetBundle::getInitialPrefabs(std::vector<AssetId>& prefabs) {
  prefabs = initial_prefabs;
}

bool AssetBundle::isAssetRegistered(AssetId id) {
  return asset_lookup.find(id) != asset_lookup.end();
}

bool AssetBundle::loadAsset(const SerializedAsset** asset, AssetId id) {
  // TODO(marceline-cramer) Better error checking and logging

  auto stored_asset = asset_lookup.find(id)->second;
  auto lump_index = stored_asset.lump_index;

  auto& cached_lump = lump_cache[lump_index];

  if (cached_lump.lump == nullptr) {
    cached_lump.lump =
        new AssetLump(bundle_root / generateLumpName(lump_index));

    if (!cached_lump.lump->assertFileSize(cached_lump.file_size)) {
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
