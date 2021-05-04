// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuBuffer.h"

#include <cstring>

#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

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

  // Save old allocation for if we need to copy
  VmaAllocation old_allocation = allocation;
  VkBuffer old_buffer = buffer;
  VmaAllocationInfo old_info = allocation_info;

  VkBufferCreateInfo buffer_ci{};
  buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_ci.size = target_size;
  buffer_ci.usage = buffer_usage_flags;
  buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocation_ci{};
  allocation_ci.usage = memory_usage;

  if (memory_usage == VMA_MEMORY_USAGE_CPU_TO_GPU) {
    allocation_ci.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  } else {
    if (old_allocation != nullptr) {
      vmaDestroyBuffer(gpu->allocator, old_buffer, old_allocation);
    }

    old_allocation = nullptr;
  }

  if (vmaCreateBuffer(gpu->allocator, &buffer_ci, &allocation_ci, &buffer,
                      &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer.");
  }

  if (old_allocation != nullptr) {
    if (memory_usage == VMA_MEMORY_USAGE_CPU_TO_GPU && buffer_size > 0) {
      memcpy(allocation_info.pMappedData, old_info.pMappedData, buffer_size);
    }

    vmaDestroyBuffer(gpu->allocator, old_buffer, old_allocation);
  }

  buffer_size = target_size;
}

}  // namespace core
}  // namespace mondradiko
