// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/shapes/Shape.h"

namespace mondradiko {
namespace core {

class AabbShape : public Shape<AabbShape> {
 public:
  vec3 min_bound;
  vec3 max_bound;
};

}  // namespace core
}  // namespace mondradiko
