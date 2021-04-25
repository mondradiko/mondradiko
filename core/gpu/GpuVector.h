// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <cstring>
#include <typeinfo>

#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

class GpuVector : public GpuBuffer {
 public:
  GpuVector(GpuInstance* gpu, size_t element_size,
            VkBufferUsageFlags usage_flags)
      : GpuBuffer(gpu, element_size, usage_flags, VMA_MEMORY_USAGE_CPU_TO_GPU) {
    if (usage_flags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
      size_t uniform_alignment = gpu->physical_device_properties.limits
                                     .minUniformBufferOffsetAlignment;
      element_granularity =
          (element_size / uniform_alignment) + uniform_alignment;
    } else {
      element_granularity = element_size;
    }
  }

  template <typename ElementType>
  void writeElement(uint32_t, const ElementType&);

  size_t getGranularity() { return element_granularity; }

  void writeData(void*) = delete;

 private:
  size_t element_granularity;
};

template <typename ElementType>
void GpuVector::writeElement(uint32_t index, const ElementType& element) {
  if (sizeof(ElementType) > element_granularity) {
    log_err_fmt("Element size %lu of type %s exceeds granularity of %lu",
                sizeof(ElementType), typeid(ElementType).name(),
                element_granularity);
    return;
  }

  size_t needed_size = (index + 1) * element_granularity;

  if (needed_size > buffer_size) {
    size_t larger_size = buffer_size << 1;
    reserve(needed_size > larger_size ? needed_size : larger_size);
  }

  memcpy(static_cast<char*>(allocation_info.pMappedData) +
             index * element_granularity,
         reinterpret_cast<const char*>(&element), sizeof(ElementType));
}

}  // namespace core
}  // namespace mondradiko
