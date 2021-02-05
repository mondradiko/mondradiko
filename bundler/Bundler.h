// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <map>
#include <string>

#include "types/assets/SerializedAsset_generated.h"
#include "assets/saving/AssetBundleBuilder.h"
#include "bundler/ConverterInterface.h"
#include "lib/include/toml_headers.h"

namespace mondradiko {

class Bundler {
 public:
  explicit Bundler(const std::filesystem::path&);
  ~Bundler();

  assets::AssetId addAsset(ConverterInterface::AssetBuilder*,
                           ConverterInterface::AssetOffset);
  void addConverter(std::string, const ConverterInterface*);
  void bundle();

 private:
  std::filesystem::path manifest_path;
  std::filesystem::path source_root;
  std::filesystem::path bundle_root;

  toml::value manifest;

  assets::AssetBundleBuilder* bundle_builder = nullptr;

  std::map<std::string, const ConverterInterface*> converters;
};

}  // namespace mondradiko
