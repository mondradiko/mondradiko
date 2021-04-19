// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "types/protocol/ProtocolTypes.h"

namespace mondradiko {
namespace protocol {

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

void GlmToQuat(Quaternion* dst, const glm::quat& src) {
  auto v = dst->mutable_v();
  v->Mutate(0, src.w);
  v->Mutate(1, src.x);
  v->Mutate(2, src.y);
  v->Mutate(3, src.z);
}

}  // namespace protocol
}  // namespace mondradiko
