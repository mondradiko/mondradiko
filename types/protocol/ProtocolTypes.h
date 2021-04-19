// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/glm_headers.h"
#include "types/protocol/types_generated.h"

namespace mondradiko {
namespace protocol {

// Global helper functions
void GlmToVec2(Vec2*, const glm::vec2&);
void GlmToVec3(Vec3*, const glm::vec3&);
void GlmToQuat(Quaternion*, const glm::quat&);

}  // namespace protocol
}  // namespace mondradiko
