// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "converter/ConverterInterface.h"
#include "lib/include/toml_headers.h"

namespace mondradiko {
namespace converter {

// Forward declarations
class BundlerInterface;

class PrefabBuilder : public ConverterInterface {
 public:
  PrefabBuilder(BundlerInterface*);

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, const toml::table&) const final;

 private:
  BundlerInterface* bundler;
};

}  // namespace converter
}  // namespace mondradiko
