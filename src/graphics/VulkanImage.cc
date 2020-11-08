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

namespace mondradiko {

VulkanImage::VulkanImage(VulkanInstance* vulkan_instance, VkFormat format,
                         uint32_t width, uint32_t height,
                         VkImageUsageFlags image_usage_flags,
                         VmaMemoryUsage memory_usage)
    : format(format),
      layout(VK_IMAGE_LAYOUT_UNDEFINED),
      width(height),
      height(height),
      vulkan_instance(vulkan_instance) {
  VkImageCreateInfo imageCreateInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
      .imageType = VK_IMAGE_TYPE_2D,
      .format = format,
      .extent = {.width = width, .height = height, .depth = 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = image_usage_flags,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .initialLayout = layout};

  VmaAllocationCreateInfo allocationCreateInfo{
      .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT, .usage = memory_usage};

  if (vmaCreateImage(vulkan_instance->allocator, &imageCreateInfo,
                     &allocationCreateInfo, &image, &allocation,
                     &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan image.");
  }
}

VulkanImage::~VulkanImage() {
  if (view != VK_NULL_HANDLE)
    vkDestroyImageView(vulkan_instance->device, view, nullptr);
  if (allocation != nullptr)
    vmaDestroyImage(vulkan_instance->allocator, image, allocation);
}

void VulkanImage::writeData(void* src) {
  // TODO(marceline-cramer) This function is bad, please replace
  // Consider a streaming job system for all static GPU assets
  VulkanBuffer stage(vulkan_instance, allocation_info.size,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VMA_MEMORY_USAGE_CPU_TO_GPU);
  stage.writeData(src);

  VkCommandBuffer commandBuffer = vulkan_instance->beginSingleTimeCommands();

  VkBufferImageCopy region{
      .bufferOffset = 0,
      .bufferRowLength = 0,
      .bufferImageHeight = 0,
      .imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                           .mipLevel = 0,
                           .baseArrayLayer = 0,
                           .layerCount = 1},
      .imageOffset = {0, 0, 0},
      .imageExtent = {width, height, 1}};

  vkCmdCopyBufferToImage(commandBuffer, stage.buffer, image, layout, 1,
                         &region);

  vulkan_instance->endSingleTimeCommands(commandBuffer);
}

void VulkanImage::transitionLayout(VkImageLayout targetLayout) {
  VkImageMemoryBarrier barrier{
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .oldLayout = layout,
      .newLayout = targetLayout,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image,
      .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1}};

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      targetLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             targetLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    log_err("Unsupported layout transition.");
    return;
  }

  VkCommandBuffer commandBuffer = vulkan_instance->beginSingleTimeCommands();
  layout = targetLayout;
  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
  vulkan_instance->endSingleTimeCommands(commandBuffer);
}

void VulkanImage::createView() {
  VkImageViewCreateInfo viewInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1}};

  if (vkCreateImageView(vulkan_instance->device, &viewInfo, nullptr, &view) !=
      VK_SUCCESS) {
    log_ftl("Failed to create VulkanImage view.");
  }
}

}  // namespace mondradiko
