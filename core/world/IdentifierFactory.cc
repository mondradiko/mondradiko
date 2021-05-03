// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/world/IdentifierFactory.h"

#include "core/components/internal/UuidComponent.h"

namespace mondradiko {
namespace core {

IdentifierFactory::IdentifierFactory(EntityRegistry* registry)
    : registry(registry), _mersenne(_random_device()) {}

IdentifierFactory::~IdentifierFactory() {}

void IdentifierFactory::createEntityUuid(EntityId entity) {
  getEntityId(entity);  // Creates UuidComponent if none exists
}

uint64_t IdentifierFactory::getEntityUuid(EntityId entity) {
  if (!registry->has<UuidComponent>(entity)) {
    uint64_t uuid = _mersenne();
    registry->emplace<UuidComponent>(entity, uuid);
    _reverse_lookup.emplace(uuid, entity);
    return uuid;
  } else {
    return registry->get<UuidComponent>(entity).getUuid();
  }
}

EntityId IdentifierFactory::getEntityId(uint64_t uuid) {
  auto iter = _reverse_lookup.find(uuid);
  if (iter != _reverse_lookup.end()) {
    return iter->second;
  } else {
    return NullEntity;
  }
}

}  // namespace core
}  // namespace mondradiko
