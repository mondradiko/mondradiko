// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <map>
#include <string>

#include "converter/AssetBundleBuilder.h"
#include "converter/BundlerInterface.h"
#include "converter/ConverterInterface.h"
#include "converter/PrefabBuilder.h"
#include "lib/include/toml_headers.h"
#include "types/assets/SerializedAsset_generated.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace bundler {

class Bundler : public converter::BundlerInterface {
 public:
  explicit Bundler(const std::filesystem::path&);
  ~Bundler();

  assets::AssetId addAsset(converter::ConverterInterface::AssetBuilder*,
                           converter::ConverterInterface::AssetOffset) final;
  void addConverter(std::string, const converter::ConverterInterface*) final;
  assets::AssetId getAssetByAlias(const std::string&) final;
  std::filesystem::path getAssetPath(const toml::table&) final;
  void bundle() final;

 private:
  std::filesystem::path manifest_path;
  std::filesystem::path source_root;
  std::filesystem::path bundle_root;

  toml::value manifest;

  converter::AssetBundleBuilder* bundle_builder = nullptr;
  converter::PrefabBuilder* prefab_builder = nullptr;

  std::map<std::string, const converter::ConverterInterface*> converters;
  std::map<std::string, assets::AssetId> asset_aliases;

  types::vector<converter::ConverterInterface*> owned_converters;
};

}  // namespace bundler
}  // namespace mondradiko
