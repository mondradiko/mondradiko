// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuImage.h"

#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

GpuImage::GpuImage(GpuInstance* gpu, VkFormat format, uint32_t width,
                   uint32_t height, VkImageUsageFlags image_usage_flags,
                   VmaMemoryUsage memory_usage)
    : gpu(gpu),
      format(format),
      layout(VK_IMAGE_LAYOUT_UNDEFINED),
      width(width),
      height(height),
      level_num(1) {
  VkImageCreateInfo image_ci{};
  image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_ci.imageType = VK_IMAGE_TYPE_2D;
  image_ci.format = format;

  VkExtent3D image_extent{};
  image_extent.width = width;
  image_extent.height = height;
  image_extent.depth = 1;
  image_ci.extent = image_extent;

  image_ci.mipLevels = level_num;
  image_ci.arrayLayers = 1;
  image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
  image_ci.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_ci.usage = image_usage_flags;
  image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_ci.initialLayout = layout;

  VmaAllocationCreateInfo alloc_ci{};
  alloc_ci.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
  alloc_ci.usage = memory_usage;

  _createImage(&image_ci, &alloc_ci);
  _createView();
}

GpuImage::GpuImage(GpuInstance* gpu, VkFormat format, uint32_t width,
                   uint32_t height, GraphicsState::SampleCount sample_count,
                   VkImageUsageFlags image_usage_flags,
                   VmaMemoryUsage memory_usage)
    : gpu(gpu),
      format(format),
      layout(VK_IMAGE_LAYOUT_UNDEFINED),
      width(width),
      height(height),
      level_num(1) {
  VkImageCreateInfo image_ci{};
  image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_ci.imageType = VK_IMAGE_TYPE_2D;
  image_ci.format = format;

  VkExtent3D image_extent{};
  image_extent.width = width;
  image_extent.height = height;
  image_extent.depth = 1;
  image_ci.extent = image_extent;

  image_ci.mipLevels = level_num;
  image_ci.arrayLayers = 1;
  image_ci.samples = GraphicsState::CreateVkSampleCount(sample_count);
  image_ci.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_ci.usage = image_usage_flags;
  image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_ci.initialLayout = layout;

  VmaAllocationCreateInfo alloc_ci{};
  alloc_ci.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
  alloc_ci.usage = memory_usage;

  _createImage(&image_ci, &alloc_ci);
  _createView();
}

GpuImage::~GpuImage() {
  if (view != VK_NULL_HANDLE) vkDestroyImageView(gpu->device, view, nullptr);

  if (allocation != nullptr) vmaDestroyImage(gpu->allocator, image, allocation);
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
  subresourceRange.levelCount = level_num;
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

void GpuImage::_createImage(VkImageCreateInfo* image_ci,
                            VmaAllocationCreateInfo* alloc_ci) {
  if (vmaCreateImage(gpu->allocator, image_ci, alloc_ci, &image, &allocation,
                     &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan image");
  }
}

void GpuImage::_createView() {
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
  subresourceRange.levelCount = level_num;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;
  viewInfo.subresourceRange = subresourceRange;

  if (vkCreateImageView(gpu->device, &viewInfo, nullptr, &view) != VK_SUCCESS) {
    log_ftl("Failed to create VulkanImage view.");
  }
}

}  // namespace core
}  // namespace mondradiko
