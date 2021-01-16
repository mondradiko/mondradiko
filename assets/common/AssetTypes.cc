/**
 * @file Asset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements AssetType.h's helper functions.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/common/AssetTypes.h"

namespace mondradiko {
namespace assets {

const char* getAssetResultString(AssetResult result) {
  switch (result) {
    case AssetResult::Success:
      return "Success";
    case AssetResult::FileNotFound:
      return "FileNotFound";
    case AssetResult::UnexpectedEof:
      return "UnexpectedEof";
    case AssetResult::BadFile:
      return "BadFile";
    case AssetResult::WrongMagic:
      return "WrongMagic";
    case AssetResult::WrongVersion:
      return "WrongVersion";
    case AssetResult::BadSize:
      return "BadSize";
    case AssetResult::BadContents:
      return "BadContents";
    case AssetResult::InvalidChecksum:
      return "InvalidChecksum";
    case AssetResult::DuplicateAsset:
      return "DuplicateAsset";
    default:
      return "Unknown result";
  }
}

std::string generateLumpName(uint32_t lump_index) {
  char buf[32];
  snprintf(buf, sizeof(buf), "lump_%04u.bin", lump_index);
  return buf;
}

glm::vec2 Vec2ToGlm(const Vec2& vec) { return glm::vec2(vec.x(), vec.y()); }

glm::vec3 Vec3ToGlm(const Vec3& vec) {
  return glm::vec3(vec.x(), vec.y(), vec.z());
}

}  // namespace assets
}  // namespace mondradiko
