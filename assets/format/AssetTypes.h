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

#include <stdint.h>

#include <string>

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

using LumpHash = uint64_t;

enum class LumpHashMethod : uint8_t { None = 0, xxHash = 1 };

enum class LumpCompressionMethod : uint8_t { None = 0, LZ4 = 1 };

// Global helper functions
std::string generateLumpName(uint32_t);
const char* getAssetResultString(AssetResult);

}  // namespace assets
}  // namespace mondradiko
