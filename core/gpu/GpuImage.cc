/**
 * @file GpuImage.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan image and its allocation.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/gpu/GpuImage.h"

#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

GpuImage::GpuImage(GpuInstance* gpu, VkFormat format, uint32_t width,
                   uint32_t height, VkImageUsageFlags image_usage_flags,
                   VmaMemoryUsage memory_usage)
    : format(format),
      layout(VK_IMAGE_LAYOUT_UNDEFINED),
      width(height),
      height(height),
      gpu(gpu) {
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
      .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
      .usage = memory_usage};

  if (vmaCreateImage(gpu->allocator, &imageCreateInfo, &allocationCreateInfo,
                     &image, &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan image.");
  }

  createView();
}

GpuImage::~GpuImage() {
  if (view != VK_NULL_HANDLE) vkDestroyImageView(gpu->device, view, nullptr);

  if (allocation != nullptr) vmaDestroyImage(gpu->allocator, image, allocation);
}

void GpuImage::writeData(const void* src) {
  // TODO(marceline-cramer) This function is bad, please replace
  // Consider a streaming job system for all static GPU assets
  GpuBuffer stage(gpu, allocation_info.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  stage.writeData(src);

  VkCommandBuffer commandBuffer = gpu->beginSingleTimeCommands();

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

  vkCmdCopyBufferToImage(commandBuffer, stage.getBuffer(), image, layout, 1,
                         &region);

  gpu->endSingleTimeCommands(commandBuffer);
}

void GpuImage::transitionLayout(VkImageLayout targetLayout) {
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

  VkCommandBuffer commandBuffer = gpu->beginSingleTimeCommands();
  layout = targetLayout;
  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);
  gpu->endSingleTimeCommands(commandBuffer);
}

void GpuImage::createView() {
  VkImageAspectFlags aspect_mask;

  switch (format) {
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT: {
      aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
      break;
    }

    default: {
      aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
      break;
    }
  }

  VkImageViewCreateInfo viewInfo{
      .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
      .image = image,
      .viewType = VK_IMAGE_VIEW_TYPE_2D,
      .format = format,
      .subresourceRange = {.aspectMask = aspect_mask,
                           .baseMipLevel = 0,
                           .levelCount = 1,
                           .baseArrayLayer = 0,
                           .layerCount = 1}};

  if (vkCreateImageView(gpu->device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
    log_ftl("Failed to create VulkanImage view.");
  }
}

}  // namespace mondradiko
