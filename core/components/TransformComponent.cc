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

#include <chrono>

namespace mondradiko {

glm::mat4 TransformComponent::getLocalTransform() {
  auto orientation = data.orientation();
  auto position = data.position();

  glm::quat converted_orientation(orientation.w(), orientation.x(), orientation.y(), orientation.z());
  glm::vec3 converted_position(position.x(), position.y(), position.z());

  glm::mat4 transform =
      glm::translate(glm::mat4(converted_orientation), converted_position);
  return transform;
}

void TransformComponent::update() {
  static auto start_time = std::chrono::high_resolution_clock::now();

  auto current_time = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   current_time - start_time)
                   .count();
  
  data.mutable_position().mutate_x(sin(time));
}

}  // namespace mondradiko
