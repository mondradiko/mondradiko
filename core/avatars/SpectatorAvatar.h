// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/avatars/Avatar.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class World;

class SpectatorAvatar : public Avatar {
 public:
  SpectatorAvatar(World*);
  ~SpectatorAvatar();

  void moveCamera(float, float, float, float, float);

  glm::vec3 getPosition() { return camera_position; }
  glm::mat4 getViewMatrix();

  // Avatar implementation
  protocol::AvatarType getProtocolType() const final;
  ProtocolBufferOffset serialize(flatbuffers::FlatBufferBuilder*) const final;
  void deserialize(const ProtocolBuffer*) final;

 private:
  World* world;

  glm::vec3 camera_position;
  float camera_pan;
  float camera_tilt;
};

}  // namespace core
}  // namespace mondradiko
