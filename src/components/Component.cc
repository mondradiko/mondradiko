/**
 * @file Component.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements reference counting of Assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "components/Component.h"

#include "scene/Entity.h"

namespace mondradiko {

Component::~Component() {
  parent->first_component = next;
  if (prev) prev->next = next;
  if (next) next->prev = prev;
}

}  // namespace mondradiko
