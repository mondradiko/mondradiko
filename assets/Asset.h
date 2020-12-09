/**
 * @file Asset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores the raw Asset data found in a lump.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <fstream>
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

// Provided by build system
#ifndef MONDRADIKO_ASSET_VERSION
#define MONDRADIKO_ASSET_VERSION (-1)
#endif

namespace mondradiko {
namespace assets {

using AssetId = uint32_t;

enum class AssetHashMethod : uint32_t { None = 0, xxHash = 1 };

enum class AssetResult {
  Success,
  FileNotFound,
  UnexpectedEof,
  BadFile,
  WrongMagic,
  WrongVersion
};

const char* getAssetResultString(AssetResult);

class Asset {
 public:
  std::string metadata;
  std::vector<uint8_t> data;

  AssetId generateId(AssetHashMethod);

 private:
};

}  // namespace assets
}  // namespace mondradiko
