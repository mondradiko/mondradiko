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
  explicit WasmConverter(BundlerInterface* bundler, assets::ScriptType script_type) : _bundler(bundler), _script_type(script_type) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, const toml::table&) const final;

 private:
  BundlerInterface* _bundler;
  const assets::ScriptType _script_type;
};

}  // namespace converter
}  // namespace mondradiko
