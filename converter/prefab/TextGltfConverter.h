// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "converter/prefab/GltfConverter.h"

namespace mondradiko {
namespace converter {

class TextGltfConverter : public GltfConverter {
 public:
  explicit TextGltfConverter(BundlerInterface* bundler)
      : GltfConverter(bundler) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, std::filesystem::path) const final;
};

}  // namespace converter
}  // namespace mondradiko
