// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuImage.h"

#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

GpuImage::GpuImage(GpuInstance* gpu, VkFormat format, uint32_t width,
                   uint32_t height, VkImageUsageFlags image_usage_flags,
                   VmaMemoryUsage memory_usage)
    : gpu(gpu),
      format(format),
      layout(VK_IMAGE_LAYOUT_UNDEFINED),
      width(width),
      height(height) {
  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = format;

  VkExtent3D image_extent{};
  image_extent.width = width;
  image_extent.height = height;
  image_extent.depth = 1;
  imageCreateInfo.extent = image_extent;

  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage = image_usage_flags;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.initialLayout = layout;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
  allocationCreateInfo.usage = memory_usage;

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

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  VkImageSubresourceLayers imageSubresource{};
  imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresource.mipLevel = 0;
  imageSubresource.baseArrayLayer = 0;
  imageSubresource.layerCount = 1;
  region.imageSubresource = imageSubresource;

  VkOffset3D image_offset{};
  image_offset.x = 0;
  image_offset.y = 0;
  image_offset.z = 0;
  region.imageOffset = image_offset;

  VkExtent3D image_extent{};
  image_extent.width = width;
  image_extent.height = height;
  image_extent.depth = 1;
  region.imageExtent = image_extent;

  vkCmdCopyBufferToImage(commandBuffer, stage.getBuffer(), image, layout, 1,
                         &region);

  gpu->endSingleTimeCommands(commandBuffer);
}

void GpuImage::transitionLayout(VkImageLayout targetLayout) {
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = layout;
  barrier.newLayout = targetLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;

  VkImageSubresourceRange subresourceRange{};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
  barrier.subresourceRange = subresourceRange;

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

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;

  VkImageSubresourceRange subresourceRange{};
  subresourceRange.aspectMask = aspect_mask;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
  viewInfo.subresourceRange = subresourceRange;

  if (vkCreateImageView(gpu->device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
    log_ftl("Failed to create VulkanImage view.");
  }
}

}  // namespace mondradiko
