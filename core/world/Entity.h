// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/entt_headers.h"

namespace mondradiko {
namespace core {

// TODO(marceline-cramer) Reflect types from protocol
using EntityId = uint32_t;
static const EntityId NullEntity = 0;
using EntityRegistry = entt::basic_registry<EntityId>;

}  // namespace core
}  // namespace mondradiko
