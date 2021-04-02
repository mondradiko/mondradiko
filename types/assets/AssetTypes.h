// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <stdint.h>

#include <string>

#include "lib/include/glm_headers.h"
#include "types/assets/types_generated.h"

namespace mondradiko {
namespace assets {

static const size_t ASSET_REGISTRY_MAX_LUMPS = 256;
static const size_t ASSET_LUMP_MAX_ASSETS = 4096;
static const size_t ASSET_LUMP_MAX_SIZE = 48 * 1024 * 1024;  // 128 MiB

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
