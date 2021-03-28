// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>

#include "converter/AssetBundleBuilder.h"
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

  virtual AssetOffset convert(AssetBuilder*, std::filesystem::path) const = 0;
};

}  // namespace converter
}  // namespace mondradiko
