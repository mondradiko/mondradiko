// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <random>

#include "core/world/Entity.h"
#include "types/containers/unordered_map.h"

namespace mondradiko {
namespace core {

class IdentifierFactory {
 public:
  IdentifierFactory(EntityRegistry*);
  ~IdentifierFactory();

  void createEntityUuid(EntityId);
  uint64_t getEntityUuid(EntityId);
  EntityId getEntityId(uint64_t);

 private:
  EntityRegistry* registry;

  std::random_device _random_device;
  std::mt19937_64 _mersenne;
  types::unordered_map<uint64_t, EntityId> _reverse_lookup;
};

}  // namespace core
}  // namespace mondradiko
