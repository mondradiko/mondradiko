/**
 * @file BinaryGltfConverter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Converts a binary glTF model into a prefab.
 * @date 2021-01-22
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "bundler/prefab/GltfConverter.h"

namespace mondradiko {

class BinaryGltfConverter : public GltfConverter {
 public:
  BinaryGltfConverter(Bundler* bundler) : GltfConverter(bundler) {}

  // ConverterInterface implementation
  AssetOffset convert(AssetBuilder*, std::filesystem::path) const final;
};

}  // namespace mondradiko
