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

  if (allocation != nullptr)
    vmaDestroyBuffer(gpu->allocator, buffer, allocation);

  allocation = nullptr;

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = target_size;
  bufferInfo.usage = buffer_usage_flags;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocationCreateInfo{};
  if (memory_usage == VMA_MEMORY_USAGE_CPU_TO_GPU)
    allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  allocationCreateInfo.usage = memory_usage;

  if (vmaCreateBuffer(gpu->allocator, &bufferInfo, &allocationCreateInfo,
                      &buffer, &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer.");
  }

  buffer_size = target_size;
}

}  // namespace core
}  // namespace mondradiko
