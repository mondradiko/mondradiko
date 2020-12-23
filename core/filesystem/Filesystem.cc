/**
 * @file Filesystem.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to the read-only files in an asset archive.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/filesystem/Filesystem.h"

#include <sstream>

#include "log/log.h"

namespace mondradiko {

Filesystem::Filesystem(const std::filesystem::path& asset_bundle_root)
    : asset_bundle(asset_bundle_root) {
  log_zone;

  auto result = asset_bundle.loadRegistry("registry.bin");
  if (result != assets::AssetResult::Success) {
    log_err(assets::getAssetResultString(result));
    log_ftl("Failed to load asset bundle registry.");
  }
}

Filesystem::~Filesystem() {}

void Filesystem::getChecksums(std::vector<assets::LumpHash>& checksums) {
  asset_bundle.getChecksums(checksums);
}

bool Filesystem::loadAsset(assets::ImmutableAsset* asset, AssetId id) {
  if (!asset_bundle.isAssetRegistered(id)) {
    log_err("Asset 0x%08x does not exist", id);
    return false;
  }

  return asset_bundle.loadAsset(asset, id);
}

}  // namespace mondradiko
