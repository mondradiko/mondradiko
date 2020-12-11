/**
 * @file Asset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores global asset type definitions.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <fstream>
#include <string>
#include <vector>

// Provided by build system
#ifndef MONDRADIKO_ASSET_VERSION
#define MONDRADIKO_ASSET_VERSION (-1)
#endif

namespace mondradiko {
namespace assets {

using AssetId = uint32_t;

// TODO(marceline-cramer) AssetLump error handling
enum class AssetResult {
  Success,
  FileNotFound,
  UnexpectedEof,
  BadFile,
  WrongMagic,
  WrongVersion,
  BadSize,
  InvalidChecksum,
  DuplicateAsset
};

const char* getAssetResultString(AssetResult);

struct Asset {
  std::string metadata;
  std::vector<uint8_t> data;
};

using LumpHash = uint32_t;

enum class LumpHashMethod : uint8_t { None = 0, xxHash = 1 };

enum class LumpCompressionMethod : uint8_t { None = 0, LZ4 = 1 };

std::string generateLumpName(uint32_t);

}  // namespace assets
}  // namespace mondradiko
