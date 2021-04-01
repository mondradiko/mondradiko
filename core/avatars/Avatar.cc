// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/avatars/Avatar.h"

namespace mondradiko {
namespace core {

void Avatar::_addEntity(EntityId new_entity) {
  _entities.push_back(new_entity);
}

}  // namespace core
}  // namespace mondradiko
