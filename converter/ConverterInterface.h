// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "converter/AssetBundleBuilder.h"
#include "lib/include/toml_headers.h"
#include "types/assets/AssetTypes.h"
#include "types/assets/SerializedAsset_generated.h"

namespace mondradiko {
namespace converter {

class ConverterInterface {
 public:
  virtual ~ConverterInterface() {}

  // Shorthand typedefs for library objects
  using AssetOffset = flatbuffers::Offset<assets::SerializedAsset>;
  using AssetBuilder = flatbuffers::FlatBufferBuilder;

  virtual AssetOffset convert(AssetBuilder*, const toml::table&) const = 0;
};

}  // namespace converter
}  // namespace mondradiko
