/**
 * @file CameraDescriptorSet.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains uniform data for each view.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_GRAPHICS_CAMERADESCRIPTORSET_H_
#define SRC_GRAPHICS_CAMERADESCRIPTORSET_H_

#include <vector>

#include "graphics/VulkanBuffer.h"
#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

struct CameraUniform {
  glm::mat4 view;
  glm::mat4 projection;
};

class CameraDescriptorSet {
 public:
  CameraDescriptorSet(VulkanInstance*, uint32_t);
  ~CameraDescriptorSet();

  void update(std::vector<XrView>*);
  void bind(VkCommandBuffer, uint32_t, VkPipelineLayout);

  VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
  std::vector<VkDescriptorSet> descriptorSets;

  VulkanBuffer* uniformBuffer = nullptr;

 private:
  VulkanInstance* vulkanInstance;
};

#endif  // SRC_GRAPHICS_CAMERADESCRIPTORSET_H_
