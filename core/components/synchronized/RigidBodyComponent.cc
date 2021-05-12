// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/components/synchronized/RigidBodyComponent.h"

#include "core/components/internal/WorldTransform.h"

namespace mondradiko {
namespace core {

RigidBodyComponent::RigidBodyComponent(const assets::RigidBodyPrefab* prefab) {
  _data.mutate_mass(prefab->mass());
}

WorldTransform RigidBodyComponent::makeWorldTransform() {
  auto transform = _rigid_body->getCenterOfMassTransform();
  auto body_position = transform.getOrigin();
  auto body_orientation = transform.getRotation();

  // TODO(marceline-cramer) Make helpers for these
  glm::vec3 position(body_position.x(), body_position.y(), body_position.z());
  glm::quat orientation(body_orientation.w(), body_orientation.x(),
                        body_orientation.y(), body_orientation.z());

  return WorldTransform(position, orientation);
}

}  // namespace core
}  // namespace mondradiko
