// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/filesystem/Filesystem.h"

#include <sstream>

#include "log/log.h"

namespace mondradiko {
namespace core {

Filesystem::Filesystem() { log_zone; }

Filesystem::~Filesystem() {
  for (auto asset_bundle : asset_bundles) {
    delete asset_bundle;
  }
}

bool Filesystem::loadAssetBundle(const std::filesystem::path& bundle_root) {
  AssetBundle* asset_bundle = new AssetBundle(bundle_root);
  auto result = asset_bundle->loadRegistry("registry.bin");
  if (result != assets::AssetResult::Success) {
    const char* error_string = assets::getAssetResultString(result);
    log_err_fmt("Failed to load asset bundle registry: %s", error_string);
    delete asset_bundle;
    return false;
  }

  asset_bundles.push_back(asset_bundle);
  return true;
}

void Filesystem::getChecksums(
    types::vector<assets::LumpHash>& local_checksums) {
  local_checksums.resize(0);

  for (auto asset_bundle : asset_bundles) {
    types::vector<assets::LumpHash> bundle_checksums;
    asset_bundle->getChecksums(bundle_checksums);
    local_checksums.insert(local_checksums.end(), bundle_checksums.begin(),
                           bundle_checksums.end());
  }
}

void Filesystem::getInitialPrefabs(types::vector<assets::AssetId>& prefabs) {
  prefabs.resize(0);

  for (auto asset_bundle : asset_bundles) {
    types::vector<assets::AssetId> bundle_prefabs;
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

types::string Filesystem::loadTextFile(const std::filesystem::path& file_path) {
  if (!std::filesystem::exists(file_path)) {
    log_err_fmt("File %s does not exist", file_path.c_str());
    return types::string(nullptr);
  }

  types::string file_data;
  std::ifstream f(file_path.c_str());

  f.seekg(0, std::ios::end);
  std::streampos length = f.tellg();
  f.seekg(0, std::ios::beg);

  file_data.resize(length);
  f.read(file_data.data(), length);
  f.close();

  return file_data;
}

types::vector<char> Filesystem::loadBinaryFile(
    const std::filesystem::path& file_path) {
  if (!std::filesystem::exists(file_path)) {
    log_err_fmt("File %s does not exist", file_path.c_str());
    return types::vector<char>();
  }

  types::vector<char> file_data;
  std::ifstream f(file_path.c_str(), std::ifstream::binary);

  f.seekg(0, std::ios::end);
  std::streampos length = f.tellg();
  f.seekg(0, std::ios::beg);

  file_data.resize(length);
  f.read(file_data.data(), length);
  f.close();

  return file_data;
}

}  // namespace core
}  // namespace mondradiko
