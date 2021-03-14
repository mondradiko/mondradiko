// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "bundler/prefab/GltfConverter.h"

namespace mondradiko {

class TextGltfConverter : public GltfConverter {
 public:
  explicit TextGltfConverter(Bundler* bundler) : GltfConverter(bundler) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, std::filesystem::path,
                      const toml::table&) const final;
};

}  // namespace mondradiko
