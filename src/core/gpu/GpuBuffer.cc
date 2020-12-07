/**
 * @file GpuBuffer.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan buffer and its allocation.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "gpu/GpuBuffer.h"

#include "gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

GpuBuffer::GpuBuffer(GpuInstance* gpu, size_t buffer_size,
                     VkBufferUsageFlags buffer_usage_flags,
                     VmaMemoryUsage memory_usage)
    : buffer_size(buffer_size), gpu(gpu) {
  VkBufferCreateInfo bufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                .size = buffer_size,
                                .usage = buffer_usage_flags,
                                .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

  VmaAllocationCreateInfo allocationCreateInfo{
      .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT, .usage = memory_usage};

  if (vmaCreateBuffer(gpu->allocator, &bufferInfo, &allocationCreateInfo,
                      &buffer, &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer.");
  }
}

GpuBuffer::~GpuBuffer() {
  if (allocation != nullptr)
    vmaDestroyBuffer(gpu->allocator, buffer, allocation);
}

void GpuBuffer::writeData(void* src) {
  // TODO(marceline-cramer) This function is bad, please replace
  // Consider a streaming job system for all static GPU assets
  memcpy(allocation_info.pMappedData, src, allocation_info.size);
}

}  // namespace mondradiko
