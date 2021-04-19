// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <stdint.h>

#include <string>

#include "lib/include/glm_headers.h"
#include "types/assets/types_generated.h"

namespace mondradiko {
namespace assets {

using AssetId = uint32_t;
constexpr AssetId NullAsset = 0;

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

void GlmToVec2(Vec2*, const glm::vec2&);
void GlmToVec3(Vec3*, const glm::vec3&);
void GlmToVec4(Vec4*, const glm::vec4&);
void GlmToQuat(Quaternion*, const glm::quat&);

}  // namespace assets
}  // namespace mondradiko
