/**
 * @file VulkanBuffer.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan buffer and its allocation.
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

#include "graphics/VulkanBuffer.h"

#include "graphics/VulkanInstance.h"
#include "log/log.h"

VulkanBuffer::VulkanBuffer(VulkanInstance* vulkanInstance, size_t bufferSize,
                           VkBufferUsageFlags bufferUsageFlags,
                           VmaMemoryUsage memoryUsage)
    : bufferSize(bufferSize), vulkanInstance(vulkanInstance) {
  VkBufferCreateInfo vertexBufferInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = bufferSize,
      .usage = bufferUsageFlags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

  VmaAllocationCreateInfo allocationCreateInfo{.usage = memoryUsage};

  if (vmaCreateBuffer(vulkanInstance->allocator, &vertexBufferInfo,
                      &allocationCreateInfo, &buffer, &allocation,
                      &allocationInfo) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer.");
  }
}

VulkanBuffer::~VulkanBuffer() {
  if (allocation != nullptr)
    vmaDestroyBuffer(vulkanInstance->allocator, buffer, allocation);
}

void VulkanBuffer::writeData(void* src) {
  void* dst;
  vkMapMemory(vulkanInstance->device, allocationInfo.deviceMemory,
              allocationInfo.offset, bufferSize, 0, &dst);
  memcpy(dst, src, bufferSize);
  vkUnmapMemory(vulkanInstance->device, allocationInfo.deviceMemory);
}
