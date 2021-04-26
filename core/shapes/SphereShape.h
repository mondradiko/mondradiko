// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/shapes/Shape.h"

namespace mondradiko {
namespace core {

class SphereShape : public Shape<SphereShape> {
 public:
  double radius;
};

}  // namespace core
}  // namespace mondradiko
