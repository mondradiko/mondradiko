/**
 * @file WasmConverter.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Converts a Wasm file into a ScriptAsset.
 * @date 2021-01-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "bundler/ConverterInterface.h"

namespace mondradiko {

class WasmConverter : public ConverterInterface {
 public:
  WasmConverter(Bundler* bundler) : _bundler(bundler) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, std::filesystem::path) const final;

 private:
  Bundler* _bundler;
};

}  // namespace mondradiko
