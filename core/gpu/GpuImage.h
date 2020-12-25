/**
 * @file GpuImage.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan image and its allocation.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/common/vulkan_headers.h"

namespace mondradiko {

// Forward declarations
class GpuBuffer;
class GpuInstance;

class GpuImage {
 public:
  GpuImage(GpuInstance*, VkFormat, uint32_t, uint32_t, VkImageUsageFlags,
           VmaMemoryUsage);
  GpuImage(GpuInstance*, ktxTexture*, VkImageUsageFlags);
  ~GpuImage();

  // void writeData(void*);
  void transitionLayout(VkImageLayout);

  VkFormat format;
  VkImageLayout layout;
  uint32_t width;
  uint32_t height;
  VmaAllocation allocation = nullptr;
  VmaAllocationInfo allocation_info;
  VkImage image = VK_NULL_HANDLE;

  bool using_ktx = false;
  ktxVulkanTexture ktx_texture;

  VkImageView view = VK_NULL_HANDLE;

 private:
  GpuInstance* gpu;

  void createView();
};

}  // namespace mondradiko
