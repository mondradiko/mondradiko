// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>

#include "assets/common/AssetTypes.h"
#include "bundler/AssetBundleBuilder.h"
#include "lib/include/toml_headers.h"
#include "types/assets/SerializedAsset_generated.h"

namespace mondradiko {

// Forward declarations
class Bundler;

class ConverterInterface {
 public:
  virtual ~ConverterInterface() {}

  // Shorthand typedefs for library objects
  using AssetOffset = flatbuffers::Offset<assets::SerializedAsset>;
  using AssetBuilder = flatbuffers::FlatBufferBuilder;

  virtual AssetOffset convert(AssetBuilder*, std::filesystem::path,
                              const toml::value&) const = 0;
};

}  // namespace mondradiko
