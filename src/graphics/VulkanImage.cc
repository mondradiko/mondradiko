/**
 * @file VulkanImage.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan image and its allocation.
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

#include "graphics/VulkanImage.h"

#include "graphics/VulkanInstance.h"
#include "log/log.h"

VulkanImage::VulkanImage(VulkanInstance* vulkanInstance, VkFormat format,
                         uint32_t width, uint32_t height,
                         VkImageUsageFlags imageUsage,
                         VmaMemoryUsage memoryUsage)
    : format(format), width(height), height(height) {
  VkImageCreateInfo imageCreateInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = format,
      .extent = {.width = width, .height = height, .depth = 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = imageUsage,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

  VmaAllocationCreateInfo allocationCreateInfo{.usage = memoryUsage};

  if (vmaCreateImage(vulkanInstance->allocator, &imageCreateInfo,
                     &allocationCreateInfo, &image, &allocation,
                     &allocationInfo) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan image.");
  }
}

VulkanImage::~VulkanImage() {
  if (allocation != nullptr)
    vmaDestroyImage(vulkanInstance->allocator, image, allocation);
}
