// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/shapes/Shape.h"

namespace mondradiko {
namespace core {

class IntAabbShape : public Shape<IntAabbShape> {
 public:
  ivec3 min_bound;
  ivec3 max_bound;
};

}  // namespace core
}  // namespace mondradiko
