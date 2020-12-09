/**
 * @file AssetLump.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores a contiguous set of Assets in binary.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/Asset.h"

namespace mondradiko {
namespace assets {

using LumpHash = uint32_t;

enum class LumpHashMethod : uint32_t {
  None = 0,
  xxHash = 1
};

enum class LumpCompressionMethod : uint32_t {
  None = 0,
  LZ4 = 1
};

class AssetLump {
 public:
  AssetLump(const char*, LumpHashMethod, LumpHash, LumpCompressionMethod);

 private:
};

}  // namespace assets
}  // namespace mondradiko
