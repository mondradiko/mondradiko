// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <cstdint>

#include "core/components/InternalComponent.h"

namespace mondradiko {
namespace core {

class UuidComponent : public InternalComponent {
 public:
  UuidComponent(uint64_t uuid) : _uuid(uuid) {}

  uint64_t getUuid() { return _uuid; }

 private:
  uint64_t _uuid;
};

}  // namespace core
}  // namespace mondradiko
