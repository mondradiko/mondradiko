// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bundler/Bundler.h"

#include <string>

#include "bundler/ConverterInterface.h"
#include "log/log.h"

namespace mondradiko {

Bundler::Bundler(const std::filesystem::path& _manifest_path)
    : manifest_path(_manifest_path) {
  if (std::filesystem::is_directory(manifest_path)) {
    manifest_path = manifest_path / "bundler-manifest.toml";
  }

  if (!std::filesystem::exists(manifest_path)) {
    log_ftl("Manifest path %s does not exist", manifest_path.c_str());
  }

  source_root = manifest_path.parent_path();
  bundle_root = "./";

  log_dbg("Bundler manifest: %s", manifest_path.c_str());
  log_dbg("Bundler source dir: %s", source_root.c_str());
  log_dbg("Bundler bundle dir: %s", bundle_root.c_str());

  bundle_builder = new assets::AssetBundleBuilder(bundle_root);

  manifest = toml::parse(manifest_path);
}

Bundler::~Bundler() {
  if (bundle_builder != nullptr) delete bundle_builder;
}

assets::AssetId Bundler::addAsset(
    ConverterInterface::AssetBuilder* fbb,
    ConverterInterface::AssetOffset asset_offset) {
  assets::AssetId asset_id;
  bundle_builder->addAsset(&asset_id, fbb, asset_offset);
  return asset_id;
}

void Bundler::addConverter(std::string file_format,
                           const ConverterInterface* converter) {
  converters.emplace(file_format, converter);
}

void Bundler::bundle() {
  const auto assets = toml::find<toml::array>(manifest, "assets");

  for (const auto& asset_table : assets) {
    const auto& asset = asset_table.as_table();

    const auto& asset_file = asset.at("file").as_string();
    auto asset_path = source_root / asset_file.str;

    std::string asset_type;
    bool initial_prefab = false;

    {
      auto iter = asset.find("type");
      if (iter != asset.end()) {
        asset_type = iter->second.as_string().str;
      } else {
        asset_type = asset_path.extension().string().substr(1);
      }
    }

    {
      auto iter = asset.find("initial_prefab");
      if (iter != asset.end()) {
        if (asset.at("initial_prefab").as_boolean()) {
          initial_prefab = true;
        }
      }
    }

    {
      log_dbg("Converting %s asset %s", asset_type.c_str(), asset_path.c_str());

      // TODO(marceline-cramer) Converted asset caching
      auto iter = converters.find(asset_type);

      if (iter == converters.end()) {
        log_ftl("Couldn't find converter for %s", asset_type.c_str());
      }

      flatbuffers::FlatBufferBuilder fbb;
      auto asset_offset = iter->second->convert(&fbb, asset_path);

      assets::AssetId asset_id;
      bundle_builder->addAsset(&asset_id, &fbb, asset_offset);
      log_inf("Added %s asset: 0x%0lx", asset_type.c_str(), asset_id);

      if (initial_prefab) {
        bundle_builder->addInitialPrefab(asset_id);
      }
    }
  }

  bundle_builder->buildBundle("registry.bin");
}

}  // namespace mondradiko
