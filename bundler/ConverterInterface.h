/**
 * @file ConverterInterface.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for asset converter implementations.
 * @date 2021-01-22
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <filesystem>

#include "assets/common/AssetTypes.h"
#include "assets/format/SerializedAsset_generated.h"
#include "assets/saving/AssetBundleBuilder.h"

namespace mondradiko {

// Forward declarations
class Bundler;

class ConverterInterface {
 public:
  virtual ~ConverterInterface() {}

  // Shorthand typedefs for library objects
  using AssetOffset = flatbuffers::Offset<assets::SerializedAsset>;
  using AssetBuilder = flatbuffers::FlatBufferBuilder;

  virtual AssetOffset convert(AssetBuilder*, std::filesystem::path) const = 0;
};

}  // namespace mondradiko
