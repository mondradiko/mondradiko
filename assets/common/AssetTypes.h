/**
 * @file Asset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores global asset type definitions.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <stdint.h>

#include <string>

#include "assets/format/types_generated.h"
// TODO(marceline-cramer) Move header helpers into lib/
#include "core/common/glm_headers.h"

// Provided by build system
#ifndef MONDRADIKO_ASSET_VERSION
#define MONDRADIKO_ASSET_VERSION (-1)
#endif

namespace mondradiko {
namespace assets {

static const uint32_t ASSET_REGISTRY_MAX_LUMPS = 256;
static const uint32_t ASSET_LUMP_MAX_ASSETS = 4096;
static const uint32_t ASSET_LUMP_MAX_SIZE = 128 * 1024 * 1024;  // 128 MiB

// TODO(marceline-cramer) AssetLump error handling
enum class AssetResult {
  Success = 0,
  FileNotFound,
  UnexpectedEof,
  BadFile,
  InvalidBuffer,
  WrongVersion,
  BadSize,
  BadContents,
  InvalidChecksum,
  DuplicateAsset
};

// Global helper functions
std::string generateLumpName(uint32_t);
const char* getAssetResultString(AssetResult);

glm::vec2 Vec2ToGlm(const Vec2&);
glm::vec3 Vec3ToGlm(const Vec3&);

}  // namespace assets
}  // namespace mondradiko
