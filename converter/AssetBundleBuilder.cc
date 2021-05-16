// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "converter/AssetBundleBuilder.h"

#include <cstring>
#include <fstream>

#include "log/log.h"
#include "lz4frame.h"  // NOLINT
#include "lz4hc.h"     // NOLINT
#include "types/assets/Registry_generated.h"
#include "types/build_config.h"
#include "xxhash.h"  // NOLINT

namespace mondradiko {
namespace converter {

// using is ok here because it'd be inconvenient not to use it
using namespace assets;  // NOLINT

AssetBundleBuilder::AssetBundleBuilder(const std::filesystem::path& bundle_root)
    : bundle_root(bundle_root) {
  log_msg_fmt("Building asset bundle at %s", bundle_root.c_str());
}

AssetBundleBuilder::~AssetBundleBuilder() {
  log_dbg_fmt("Cleaning up asset bundle %s", bundle_root.c_str());

  for (auto& lump : lumps) {
    if (lump == nullptr) continue;

    if (lump->finalizer_thread != nullptr) {
      if (lump->finalizer_thread->joinable()) {
        log_wrn("Joining rogue lump finalizer thread");
        lump->finalizer_thread->join();
      }

      delete lump->finalizer_thread;
    }

    if (lump->data != nullptr) delete[] lump->data;

    delete lump;
  }
}

AssetResult AssetBundleBuilder::addAsset(
    AssetId* id, flatbuffers::FlatBufferBuilder* fbb,
    flatbuffers::Offset<SerializedAsset> asset_offset) {
  fbb->Finish(asset_offset);
  size_t asset_size = fbb->GetSize();

  if (asset_size > ASSET_LUMP_MAX_SIZE) {
    log_err("Asset size exceeds max asset lump size");
    return AssetResult::BadSize;
  }

  // Generate ID by hashing asset data
  *id = static_cast<AssetId>(XXH3_64bits(fbb->GetBufferPointer(), asset_size));

  if (used_ids.find(*id) != used_ids.end()) {
    log_wrn_fmt("Attempted to build asset with duplicated ID 0x%0x", *id);
    return AssetResult::DuplicateAsset;
  }

  if (lumps.size() == 0) {
    lumps.resize(1);
    lumps[0] = allocateLump(0);
  }

  uint32_t lump_index = lumps.size() - 1;

  if (lumps[lump_index]->total_size + asset_size > ASSET_LUMP_MAX_SIZE) {
    // Now that we're done with this lump, finalize it
    launchFinalizer(lumps[lump_index]);

    lump_index++;
    LumpToSave* new_lump = allocateLump(lump_index);
    lumps.push_back(new_lump);
  }

  if (lumps[lump_index]->finalizer_thread != nullptr) {
    log_err_fmt("Lump %d has already been finalized", lump_index);
    return AssetResult::BadContents;
  }

  AssetToSave new_asset;
  new_asset.id = *id;
  new_asset.size = asset_size;
  lumps[lump_index]->assets.push_back(new_asset);

  memcpy(lumps[lump_index]->data + lumps[lump_index]->total_size,
         fbb->GetBufferPointer(), asset_size);

  lumps[lump_index]->total_size += asset_size;
  used_ids.emplace(*id);

  return AssetResult::Success;
}

AssetResult AssetBundleBuilder::addBundleExport(const std::string& alias,
                                                AssetId id) {
  auto iter = used_ids.find(id);
  if (iter == used_ids.end()) {
    log_err_fmt("Attempted to export asset with unrecognized ID 0x%0x", id);
    return AssetResult::BadContents;
  }

  AssetToExport asset_export{};
  asset_export.alias = alias;
  asset_export.id = id;
  exported_assets.push_back(asset_export);

  return AssetResult::Success;
}

AssetResult AssetBundleBuilder::addInitialPrefab(AssetId prefab) {
  initial_prefabs.push_back(prefab);
  return AssetResult::Success;
}

AssetResult AssetBundleBuilder::buildBundle(const char* registry_name) {
  // Finalize the last lump we handled
  launchFinalizer(lumps.back());

  flatbuffers::FlatBufferBuilder fbb;

  std::vector<flatbuffers::Offset<LumpEntry>> lump_offsets;

  for (uint32_t lump_index = 0; lump_index < lumps.size(); lump_index++) {
    auto& lump = lumps[lump_index];

    if (lump->finalizer_thread == nullptr) {
      log_wrn_fmt("Finalizing lump %s late", lump->lump_path.c_str());
      launchFinalizer(lump);
    }

    if (lump->finalizer_thread->joinable()) {
      lump->finalizer_thread->join();
    }

    AssetEntry* asset_entries;
    auto assets_offset = fbb.CreateUninitializedVectorOfStructs(
        lump->assets.size(), &asset_entries);

    for (uint32_t i = 0; i < lump->assets.size(); i++) {
      auto& asset = lump->assets[i];
      asset_entries[i].mutate_id(asset.id);
      asset_entries[i].mutate_size(asset.size);
    }

    LumpEntryBuilder lump_entry(fbb);

    lump_entry.add_file_size(lump->total_size);
    lump_entry.add_checksum(lump->checksum);
    lump_entry.add_hash_method(lump->hash_method);
    lump_entry.add_compression_method(lump->compression_method);
    lump_entry.add_assets(assets_offset);

    lump_offsets.push_back(lump_entry.Finish());
  }

  std::vector<flatbuffers::Offset<assets::BundleExport>> bundle_exports;
  for (auto& exported_asset : exported_assets) {
    auto alias_offset = fbb.CreateString(exported_asset.alias);

    assets::BundleExportBuilder bundle_export(fbb);
    bundle_export.add_alias(alias_offset);
    bundle_export.add_id(exported_asset.id);

    bundle_exports.push_back(bundle_export.Finish());
  }

  auto bundle_exports_offset = fbb.CreateVector(bundle_exports);
  auto initial_prefabs_offset = fbb.CreateVector(
      reinterpret_cast<const uint32_t*>(initial_prefabs.data()),
      initial_prefabs.size());
  auto lumps_offset = fbb.CreateVector(lump_offsets);

  RegistryBuilder registry_builder(fbb);
  registry_builder.add_major_version(MONDRADIKO_VERSION_MAJOR);
  registry_builder.add_minor_version(MONDRADIKO_VERSION_MINOR);
  registry_builder.add_patch_version(MONDRADIKO_VERSION_PATCH);
  registry_builder.add_exports(bundle_exports_offset);
  registry_builder.add_initial_prefabs(initial_prefabs_offset);
  registry_builder.add_lumps(lumps_offset);

  fbb.Finish(registry_builder.Finish());

  auto registry_path = bundle_root / registry_name;
  std::ofstream registry_file(registry_path.c_str(), std::ofstream::binary);
  registry_file.write(reinterpret_cast<char*>(fbb.GetBufferPointer()),
                      fbb.GetSize());
  registry_file.close();

  return AssetResult::Success;
}

void AssetBundleBuilder::launchFinalizer(LumpToSave* lump) {
  if (lump->finalizer_thread != nullptr) {
    log_err("Attempting to finalize lump twice");
    return;
  }

  // TODO(marceline-cramer) Thread pool
  // TODO(marceline-cramer) Job subsystem

  std::thread* new_thread = new std::thread(
      finalizeLump, lump, default_compression, LumpHashMethod::xxHash);
  lump->finalizer_thread = new_thread;
}

AssetBundleBuilder::LumpToSave* AssetBundleBuilder::allocateLump(
    uint32_t lump_index) {
  LumpToSave* new_lump = new LumpToSave;
  new_lump->total_size = 0;
  new_lump->data = new char[ASSET_LUMP_MAX_SIZE];
  new_lump->assets.resize(0);
  new_lump->finalizer_thread = nullptr;
  new_lump->lump_path = bundle_root / generateLumpName(lump_index);
  return new_lump;
}

void AssetBundleBuilder::finalizeLump(LumpToSave* lump,
                                      LumpCompressionMethod compression_method,
                                      LumpHashMethod hash_method) {
  if (hash_method != LumpHashMethod::xxHash) {
    log_ftl("Non-xxHash hash methods are not yet supported");
  }

  lump->compression_method = compression_method;
  lump->hash_method = hash_method;

  if (compression_method == LumpCompressionMethod::LZ4) {
    // TODO(marceline-cramer) More lump compression types?
    log_dbg("Compressing lump with LZ4");

    LZ4F_preferences_t preferences;
    memset(&preferences, 0, sizeof(preferences));
    preferences.frameInfo.contentSize = lump->total_size;
    // TODO(marceline-cramer) Custom compression level from bundle manifest
    preferences.compressionLevel = LZ4HC_CLEVEL_DEFAULT;
    preferences.autoFlush = 1;
    preferences.favorDecSpeed = 1;

    size_t compressed_size =
        LZ4F_compressFrameBound(lump->total_size, &preferences);
    char* compressed_data = new char[compressed_size];

    size_t out_size =
        LZ4F_compressFrame(compressed_data, compressed_size, lump->data,
                           lump->total_size, &preferences);

    if (LZ4F_isError(out_size)) {
      log_err_fmt("LZ4 compression failed: %s", LZ4F_getErrorName(out_size));
      return;
    }

    // TODO(marceline-cramer) Stream compression data out to the file
    delete[] lump->data;
    lump->total_size = out_size;
    lump->data = compressed_data;
  } else if (compression_method != LumpCompressionMethod::None) {
    log_err("Non-LZ4 compression methods are not yet implemented");
  }

  {
    log_dbg("Hashing lump with xxHash");

    LumpHash checksum =
        static_cast<LumpHash>(XXH3_64bits(lump->data, lump->total_size));
    log_dbg_fmt("Lump has checksum 0x%0lx", checksum);
    lump->checksum = checksum;
  }

  {
    log_dbg("Writing lump data to file");

    std::ofstream lump_file(lump->lump_path.c_str(), std::ofstream::binary);
    lump_file.write(reinterpret_cast<char*>(lump->data), lump->total_size);
    lump_file.close();
  }
}

}  // namespace converter
}  // namespace mondradiko
