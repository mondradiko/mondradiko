// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/filesystem/Filesystem.h"

#include <sstream>

#include "log/log.h"

namespace mondradiko {

Filesystem::Filesystem() { log_zone; }

Filesystem::~Filesystem() {
  for (auto asset_bundle : asset_bundles) {
    delete asset_bundle;
  }
}

bool Filesystem::loadAssetBundle(const std::filesystem::path& bundle_root) {
  assets::AssetBundle* asset_bundle = new assets::AssetBundle(bundle_root);
  auto result = asset_bundle->loadRegistry("registry.bin");
  if (result != assets::AssetResult::Success) {
    log_err(assets::getAssetResultString(result));
    log_err("Failed to load asset bundle registry.");
    delete asset_bundle;
    return false;
  }

  asset_bundles.push_back(asset_bundle);
  return true;
}

void Filesystem::getChecksums(std::vector<assets::LumpHash>& local_checksums) {
  local_checksums.resize(0);

  for (auto asset_bundle : asset_bundles) {
    std::vector<assets::LumpHash> bundle_checksums;
    asset_bundle->getChecksums(bundle_checksums);
    local_checksums.insert(local_checksums.end(), bundle_checksums.begin(),
                           bundle_checksums.end());
  }
}

void Filesystem::getInitialPrefabs(std::vector<assets::AssetId>& prefabs) {
  prefabs.resize(0);

  for (auto asset_bundle : asset_bundles) {
    std::vector<assets::AssetId> bundle_prefabs;
    asset_bundle->getInitialPrefabs(bundle_prefabs);
    prefabs.insert(prefabs.end(), bundle_prefabs.begin(), bundle_prefabs.end());
  }
}

bool Filesystem::loadAsset(const assets::SerializedAsset** asset, AssetId id) {
  // TODO(marceline-cramer) The index of each asset's bundle could be cached
  for (auto asset_bundle : asset_bundles) {
    if (asset_bundle->isAssetRegistered(id)) {
      return asset_bundle->loadAsset(asset, id);
    }
  }

  log_err_fmt("Asset 0x%0dx does not exist", id);
  return false;
}

toml::value Filesystem::loadToml(const std::filesystem::path& toml_path) {
  log_dbg_fmt("Loading TOML file: %s", toml_path.c_str());
  return toml::parse(toml_path);
}

}  // namespace mondradiko
