/**
 * @file Component.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Component common definitions.
 * @date 2020-12-16
 * 
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * 
 */

#pragma once

#include "lib/entt.hpp"
#include "glm/glm.hpp"

namespace mondradiko {

using EntityId = entt::entity;
static const EntityId NullEntity = entt::null;

}  // namespace mondradiko
