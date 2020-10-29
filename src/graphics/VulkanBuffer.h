/**
 * @file VulkanBuffer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan buffer and its allocation.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_GRAPHICS_VULKANBUFFER_H_
#define SRC_GRAPHICS_VULKANBUFFER_H_

#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class VulkanBuffer {
 public:
  VulkanBuffer(VulkanInstance*, size_t, VkBufferUsageFlags, VmaMemoryUsage);
  ~VulkanBuffer();

  void writeData(void*);

  size_t bufferSize = 0;
  VmaAllocation allocation = nullptr;
  VmaAllocationInfo allocationInfo;
  VkBuffer buffer = VK_NULL_HANDLE;

 private:
  VulkanInstance* vulkanInstance;
};

}  // namespace mondradiko

#endif  // SRC_GRAPHICS_VULKANBUFFER_H_
