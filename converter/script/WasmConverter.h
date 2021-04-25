// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "converter/ConverterInterface.h"

namespace mondradiko {
namespace converter {

// Forward declarations
class BundlerInterface;

class WasmConverter : public ConverterInterface {
 public:
  explicit WasmConverter(BundlerInterface* bundler) : _bundler(bundler) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, const toml::table&) const final;

 private:
  BundlerInterface* _bundler;
};

}  // namespace converter
}  // namespace mondradiko
