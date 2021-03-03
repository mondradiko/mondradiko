// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bundler/Bundler.h"

#include <string>

#include "bundler/ConverterInterface.h"
#include "bundler/PrefabBuilder.h"
#include "log/log.h"

namespace mondradiko {

Bundler::Bundler(const std::filesystem::path& _manifest_path)
    : manifest_path(_manifest_path) {
  if (std::filesystem::is_directory(manifest_path)) {
    manifest_path = manifest_path / "bundler-manifest.toml";
  }

  if (!std::filesystem::exists(manifest_path)) {
    log_ftl_fmt("Manifest path %s does not exist", manifest_path.c_str());
  }

  source_root = manifest_path.parent_path();
  bundle_root = "./";

  log_dbg_fmt("Bundler manifest: %s", manifest_path.string().c_str());
  log_dbg_fmt("Bundler source dir: %s", source_root.string().c_str());
  log_dbg_fmt("Bundler bundle dir: %s", bundle_root.string().c_str());

  bundle_builder = new assets::AssetBundleBuilder(bundle_root);
  prefab_builder = new PrefabBuilder;

  manifest = toml::parse(manifest_path);
}

Bundler::~Bundler() {
  if (bundle_builder != nullptr) delete bundle_builder;
  if (prefab_builder != nullptr) delete prefab_builder;
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

assets::AssetId Bundler::getAssetByAlias(const std::string& alias) {
  auto iter = asset_aliases.find(alias);
  if (iter == asset_aliases.end()) {
    return assets::AssetId::NullAsset;
  }

  return iter->second;
}

void Bundler::bundle() {
  const auto assets = toml::find<toml::array>(manifest, "assets");

  for (const auto& asset_table : assets) {
    const auto& asset = asset_table.as_table();

    const auto& asset_file = asset.at("file").as_string();
    auto asset_path = source_root / asset_file.str;

    std::string asset_type;
    std::string alias = "";
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
      auto iter = asset.find("alias");
      if (iter != asset.end()) {
        alias = asset.at("alias").as_string();
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
      log_dbg_fmt("Converting %s asset %s", asset_type.c_str(),
                  asset_path.c_str());

      // TODO(marceline-cramer) Converted asset caching
      auto iter = converters.find(asset_type);

      if (iter == converters.end()) {
        log_ftl_fmt("Couldn't find converter for %s", asset_type.c_str());
      }

      flatbuffers::FlatBufferBuilder fbb;
      auto asset_offset = iter->second->convert(&fbb, asset_path);

      assets::AssetId asset_id;
      bundle_builder->addAsset(&asset_id, &fbb, asset_offset);
      log_inf_fmt("Added %s asset: 0x%0dx", asset_type.c_str(), asset_id);

      if (alias.length() > 0) {
        asset_aliases.emplace(alias, asset_id);
      }

      if (initial_prefab) {
        bundle_builder->addInitialPrefab(asset_id);
      }
    }
  }

  const auto prefabs = toml::find<toml::array>(manifest, "prefabs");

  for (const auto& prefab_table : prefabs) {
    const auto& prefab = prefab_table.as_table();

    assets::AssetId prefab_id = prefab_builder->buildPrefab(this, prefab);

    auto iter = prefab.find("initial_prefab");
    if (iter != prefab.end()) {
      if (prefab.at("initial_prefab").as_boolean()) {
        bundle_builder->addInitialPrefab(prefab_id);
      }
    }
  }

  bundle_builder->buildBundle("registry.bin");
}

}  // namespace mondradiko
