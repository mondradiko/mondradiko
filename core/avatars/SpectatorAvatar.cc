// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/avatars/SpectatorAvatar.h"

#include <cmath>

#include "core/components/internal/PointerComponent.h"
#include "core/components/scriptable/TransformComponent.h"
#include "core/world/World.h"
#include "types/protocol/SpectatorAvatar_generated.h"

namespace mondradiko {
namespace core {

SpectatorAvatar::SpectatorAvatar(World* world) : world(world) {
  camera_position = glm::vec3(0.0, 0.0, 0.0);
  camera_pan = 0.0;
  camera_tilt = 0.0;

  _self_id = world->registry.create();
  _addEntity(_self_id);

  world->registry.emplace<PointerComponent>(_self_id, glm::vec3(0.0),
                                            glm::vec3(0.0, 0.0, 1.0));
}

SpectatorAvatar::~SpectatorAvatar() { world->registry.destroy(_self_id); }

void SpectatorAvatar::moveCamera(float pan, float tilt, float truck,
                                 float dolly, float boom) {
  camera_pan += pan;
  camera_tilt += tilt;

  if (camera_tilt <= -M_PI_2) camera_tilt = -M_PI_2;
  if (camera_tilt >= M_PI_2) camera_tilt = M_PI_2;

  // Truck direction from straight down
  glm::vec2 horizontal_component = glm::vec2(sin(camera_pan), -cos(camera_pan));
  // Truck direction from the side
  glm::vec2 vertical_component = glm::vec2(cos(camera_tilt), -sin(camera_tilt));
  // Composite to get forward direction
  glm::vec3 truck_direction = glm::vec3(
      horizontal_component.x * vertical_component.x, vertical_component.y,
      horizontal_component.y * vertical_component.x);

  glm::vec3 dolly_direction =
      glm::vec3(-cos(camera_pan), 0.0, -sin(camera_pan));

  camera_position += truck_direction * truck;
  camera_position += dolly_direction * dolly;
  camera_position.y += boom;

  _updateTransform();
}

glm::mat4 SpectatorAvatar::getViewMatrix() {
  glm::quat camera_orientation =
      glm::angleAxis(camera_tilt, glm::vec3(1.0, 0.0, 0.0)) *
      glm::angleAxis(camera_pan, glm::vec3(0.0, 1.0, 0.0));
  return glm::translate(glm::mat4(camera_orientation), -camera_position);
}

protocol::AvatarType SpectatorAvatar::getProtocolType() const {
  return protocol::AvatarType::Spectator;
}

SpectatorAvatar::ProtocolBufferOffset SpectatorAvatar::serialize(
    flatbuffers::FlatBufferBuilder* protocol_builder) const {
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Clear();

  protocol::Vec3 position;
  protocol::GlmToVec3(&position, camera_position);

  protocol::SpectatorAvatarBuilder spectator_avatar(fbb);
  spectator_avatar.add_position(&position);
  spectator_avatar.add_pan(camera_pan);
  spectator_avatar.add_tilt(camera_tilt);
  auto spectator_avatar_offset = spectator_avatar.Finish();

  fbb.Finish(spectator_avatar_offset);
  return protocol_builder->CreateVector(fbb.GetBufferPointer(), fbb.GetSize());
}

void SpectatorAvatar::deserialize(const ProtocolBuffer* protocol_data) {
  flatbuffers::Verifier verifier(protocol_data->data(), protocol_data->size());
  if (!protocol::VerifySpectatorAvatarBuffer(verifier)) {
    log_err("SpectatorAvatar validation failed");
    return;
  }

  auto spectator_avatar = protocol::GetSpectatorAvatar(protocol_data->data());

  if (spectator_avatar->position() != nullptr) {
    // TODO(marceline-cramer) Helpers for these
    camera_position = glm::make_vec3(spectator_avatar->position()->v()->data());
  }

  camera_pan = spectator_avatar->pan();
  camera_tilt = spectator_avatar->tilt();

  _updateTransform();
}

void SpectatorAvatar::_updateTransform() {
  auto position = camera_position;

  glm::quat orientation =
      glm::angleAxis(camera_tilt, glm::vec3(0.0, 0.0, 1.0)) *
      glm::angleAxis(camera_pan, glm::vec3(0.0, 1.0, 0.0));

  world->registry.emplace_or_replace<TransformComponent>(_self_id, position,
                                                         orientation);
}

}  // namespace core
}  // namespace mondradiko
