// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <filesystem>
#include <string>

#include "converter/AssetBundleBuilder.h"
#include "converter/ConverterInterface.h"
#include "types/assets/SerializedAsset_generated.h"

namespace mondradiko {

// Forward declarations
class PrefabBuilder;

class BundlerInterface {
 public:
  virtual ~BundlerInterface() {}

  virtual assets::AssetId addAsset(ConverterInterface::AssetBuilder*,
                                   ConverterInterface::AssetOffset) = 0;
  virtual void addConverter(std::string, const ConverterInterface*) = 0;
  virtual assets::AssetId getAssetByAlias(const std::string&) = 0;
  virtual void bundle() = 0;
};

}  // namespace mondradiko
