/**
 * @file GpuBuffer.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages a Vulkan buffer and its allocation.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
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