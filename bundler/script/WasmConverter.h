// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "bundler/ConverterInterface.h"

namespace mondradiko {

class WasmConverter : public ConverterInterface {
 public:
  explicit WasmConverter(Bundler* bundler) : _bundler(bundler) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, std::filesystem::path,
                      const toml::value&) const final;

 private:
  Bundler* _bundler;
};

}  // namespace mondradiko
