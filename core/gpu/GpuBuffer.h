// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"

namespace mondradiko {

// Forward declarations
class GpuInstance;

class GpuBuffer {
 public:
  GpuBuffer(GpuInstance* gpu, size_t initial_size,
            VkBufferUsageFlags buffer_usage_flags)
      : GpuBuffer(gpu, initial_size, buffer_usage_flags,
                  VMA_MEMORY_USAGE_CPU_TO_GPU) {}
  ~GpuBuffer();

  VkBuffer getBuffer() const { return buffer; }
  size_t getBufferSize() const { return buffer_size; }

  // TODO(marceline-cramer) Get rid of this method
  void writeData(const void*);

 protected:
  GpuBuffer(GpuInstance*, size_t, VkBufferUsageFlags, VmaMemoryUsage);

  /**
   * @warning This method may potentially recreate the buffer handle,
   * making any former references to this buffer invalid.
   *
   */
  void reserve(size_t);

  size_t buffer_size = 0;
  VkBufferUsageFlags buffer_usage_flags;
  VmaMemoryUsage memory_usage;
  VmaAllocation allocation = nullptr;
  VmaAllocationInfo allocation_info;
  VkBuffer buffer = VK_NULL_HANDLE;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko
