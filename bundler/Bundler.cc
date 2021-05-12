// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "bundler/Bundler.h"

#include <string>

#include "converter/ConverterInterface.h"
#include "converter/prefab/BinaryGltfConverter.h"
#include "converter/prefab/PrefabBuilder.h"
#include "converter/prefab/TextGltfConverter.h"
#include "converter/script/WasmConverter.h"
#include "log/log.h"

namespace mondradiko {
namespace bundler {

// using is ok here because it'd be inconvenient not to use it
using namespace converter;  // NOLINT

Bundler::Bundler(const std::filesystem::path& _manifest_path)
    : manifest_path(_manifest_path) {
  {  // Create converters
    auto binary_gltf_converter = new BinaryGltfConverter(this);
    owned_converters.push_back(binary_gltf_converter);
    addConverter("glb", binary_gltf_converter);
    addConverter("vrm", binary_gltf_converter);

    auto text_gltf_converter = new TextGltfConverter(this);
    owned_converters.push_back(text_gltf_converter);
    addConverter("gltf", text_gltf_converter);

    auto binary_wasm_converter =
        new WasmConverter(this, assets::ScriptType::WasmBinary);
    owned_converters.push_back(binary_wasm_converter);
    addConverter("wasm", binary_wasm_converter);

    auto prefab_builder = new PrefabBuilder(this);
    owned_converters.push_back(prefab_builder);
    addConverter("prefab", prefab_builder);

    auto text_wasm_converter =
        new WasmConverter(this, assets::ScriptType::WasmText);
    owned_converters.push_back(text_wasm_converter);
    addConverter("wat", text_wasm_converter);
  }

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

  bundle_builder = new AssetBundleBuilder(bundle_root);

  manifest = toml::parse(manifest_path);

  {  // Select compression method
    assets::LumpCompressionMethod compression_method;
    compression_method = assets::LumpCompressionMethod::None;

    const auto& bundle_table = toml::find<toml::table>(manifest, "bundle");

    auto iter = bundle_table.find("compression");
    if (iter != bundle_table.end()) {
      std::string compression_name = iter->second.as_string();
      if (compression_name == "LZ4") {
        compression_method = assets::LumpCompressionMethod::LZ4;
      } else if (compression_name != "none") {
        log_ftl_fmt("Invalid compression method %s", compression_name.c_str());
      }
    }

    bundle_builder->setDefaultCompressionMethod(compression_method);
  }
}

Bundler::~Bundler() {
  for (auto converter : owned_converters) {
    delete converter;
  }

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

assets::AssetId Bundler::getAssetByAlias(const std::string& alias) {
  auto iter = asset_aliases.find(alias);
  if (iter == asset_aliases.end()) {
    return assets::NullAsset;
  }

  return iter->second;
}

std::filesystem::path Bundler::getAssetPath(const toml::table& asset) {
  const auto& asset_file = asset.at("file").as_string();
  auto asset_path = source_root / asset_file.str;
  return asset_path;
}

void Bundler::bundle() {
  const auto assets = toml::find<toml::array>(manifest, "assets");

  for (const auto& asset_table : assets) {
    const auto& asset = asset_table.as_table();

    std::string asset_type;
    std::string alias = "";
    bool initial_prefab = false;

    {
      auto iter = asset.find("type");
      if (iter != asset.end()) {
        asset_type = iter->second.as_string().str;
      } else {
        auto asset_path = getAssetPath(asset);
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
      log_dbg_fmt("Converting %s asset", asset_type.c_str());

      // TODO(marceline-cramer) Converted asset caching
      auto iter = converters.find(asset_type);

      if (iter == converters.end()) {
        log_ftl_fmt("Couldn't find converter for %s", asset_type.c_str());
      }

      flatbuffers::FlatBufferBuilder fbb;
      auto asset_offset = iter->second->convert(&fbb, asset);

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

  for (auto& iter : asset_aliases) {
    bundle_builder->addBundleExport(iter.first, iter.second);
  }

  bundle_builder->buildBundle("registry.bin");
}

}  // namespace bundler
}  // namespace mondradiko
