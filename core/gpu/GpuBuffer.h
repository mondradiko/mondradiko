/**
 * @file GpuBuffer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan buffer and its allocation.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/common/api_headers.h"

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

  VkBuffer getBuffer() { return buffer; }

  // TODO(marceline-cramer) Get rid of this method
  void writeData(void*);

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
