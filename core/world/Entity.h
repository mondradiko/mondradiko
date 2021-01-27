/**
 * @file Entity.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Defines Entity-related types.
 * @date 2020-12-25
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "lib/include/entt_headers.h"

namespace mondradiko {

// TODO(marceline-cramer) Reflect types from protocol
using EntityId = uint32_t;
static const EntityId NullEntity = 0;
using EntityRegistry = entt::basic_registry<EntityId>;

}  // namespace mondradiko
