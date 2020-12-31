/**
 * @file TransformComponent.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-27
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/components/TransformComponent.h"

#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

glm::mat4 TransformComponent::getLocalTransform() {
  auto orientation = _data.orientation();
  auto position = _data.position();

  glm::quat converted_orientation(orientation.w(), orientation.x(),
                                  orientation.y(), orientation.z());
  glm::vec3 converted_position(position.x(), position.y(), position.z());

  glm::mat4 transform =
      glm::translate(glm::mat4(converted_orientation), converted_position);
  return transform;
}

// Template specialization to build UpdateComponents event
template <>
void buildUpdateComponents<protocol::TransformComponent>(
    protocol::UpdateComponentsBuilder* update_components,
    flatbuffers::Offset<
        flatbuffers::Vector<const protocol::TransformComponent*>> components) {
  update_components->add_type(protocol::ComponentType::TransformComponent);
  update_components->add_transform(components);
}

}  // namespace mondradiko
