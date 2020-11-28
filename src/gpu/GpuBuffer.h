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

#ifndef SRC_GPU_GPUBUFFER_H_
#define SRC_GPU_GPUBUFFER_H_

#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class GpuBuffer {
 public:
  GpuBuffer(GpuInstance*, size_t, VkBufferUsageFlags, VmaMemoryUsage);
  ~GpuBuffer();

  void writeData(void*);

  size_t buffer_size = 0;
  VmaAllocation allocation = nullptr;
  VmaAllocationInfo allocation_info;
  VkBuffer buffer = VK_NULL_HANDLE;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko

#endif  // SRC_GPU_GPUBUFFER_H_
