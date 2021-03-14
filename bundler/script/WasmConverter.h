// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "bundler/ConverterInterface.h"
#include "wasm.h"      // NOLINT
#include "wasmtime.h"  // NOLINT

namespace mondradiko {

class WasmConverter : public ConverterInterface {
 public:
  explicit WasmConverter(Bundler*);
  ~WasmConverter();

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, std::filesystem::path,
                      const toml::table&) const final;

 private:
  Bundler* _bundler;

  wasm_engine_t* engine = nullptr;
  wasm_store_t* store = nullptr;
};

}  // namespace mondradiko
