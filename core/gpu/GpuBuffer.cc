// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuBuffer.h"

#include <cstring>

#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

GpuBuffer::GpuBuffer(GpuInstance* gpu, size_t initial_size,
                     VkBufferUsageFlags buffer_usage_flags,
                     VmaMemoryUsage memory_usage)
    : buffer_usage_flags(buffer_usage_flags),
      memory_usage(memory_usage),
      gpu(gpu) {
  reserve(initial_size);
}

GpuBuffer::~GpuBuffer() {
  if (allocation != nullptr)
    vmaDestroyBuffer(gpu->allocator, buffer, allocation);
}

void GpuBuffer::reserve(size_t target_size) {
  if (target_size <= buffer_size) return;

  if (allocation != nullptr)
    vmaDestroyBuffer(gpu->allocator, buffer, allocation);

  allocation = nullptr;

  VkBufferCreateInfo bufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                .size = target_size,
                                .usage = buffer_usage_flags,
                                .sharingMode = VK_SHARING_MODE_EXCLUSIVE};

  VmaAllocationCreateInfo allocationCreateInfo{
      .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT, .usage = memory_usage};

  if (vmaCreateBuffer(gpu->allocator, &bufferInfo, &allocationCreateInfo,
                      &buffer, &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer.");
  }

  buffer_size = target_size;
}

void GpuBuffer::writeData(const void* src) {
  // TODO(marceline-cramer) This function is bad, please replace
  // Consider a streaming job system for all static GPU assets
  memcpy(allocation_info.pMappedData, src, allocation_info.size);
}

}  // namespace mondradiko
