// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "types/assets/AssetTypes.h"

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
    case AssetResult::InvalidBuffer:
      return "InvalidBuffer";
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

void GlmToVec2(Vec2* dst, const glm::vec2& src) {
  auto v = dst->mutable_v();
  v->Mutate(0, src.x);
  v->Mutate(1, src.y);
}

void GlmToVec3(Vec3* dst, const glm::vec3& src) {
  auto v = dst->mutable_v();
  v->Mutate(0, src.x);
  v->Mutate(1, src.y);
  v->Mutate(2, src.z);
}

void GlmToVec4(Vec4* dst, const glm::vec4& src) {
  auto v = dst->mutable_v();
  v->Mutate(0, src.x);
  v->Mutate(1, src.y);
  v->Mutate(2, src.z);
}

void GlmToQuat(Quaternion* dst, const glm::quat& src) {
  auto v = dst->mutable_v();
  v->Mutate(0, src.w);
  v->Mutate(1, src.x);
  v->Mutate(2, src.y);
  v->Mutate(3, src.z);
}

}  // namespace assets
}  // namespace mondradiko
