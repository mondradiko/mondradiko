// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "lib/include/vulkan_headers.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

// TODO(marceline-cramer) Named descriptors
// TODO(marceline-cramer) Reflection

// Forward declarations
class GpuBuffer;
class GpuDescriptorSetLayout;
class GpuImage;
class GpuInstance;
class GpuVector;

class GpuDescriptorSet {
 public:
  GpuDescriptorSet(GpuInstance*, GpuDescriptorSetLayout*, VkDescriptorSet);

  void updateBuffer(uint32_t, GpuBuffer*);
  void updateDynamicBuffer(uint32_t, GpuVector*);
  void updateStorageBuffer(uint32_t, const GpuBuffer*);
  void updateImage(uint32_t, const GpuImage*);

  void updateDynamicOffset(uint32_t, uint32_t);

  void cmdBind(VkCommandBuffer, VkPipelineLayout, uint32_t) const;

 private:
  GpuInstance* gpu;

  // Only GpuDescriptorPools are allowed to destroy sets
  friend class GpuDescriptorPool;
  ~GpuDescriptorSet();

  GpuDescriptorSetLayout* set_layout;
  VkDescriptorSet descriptor_set;
  types::vector<uint32_t> dynamic_offset_granularity;
  types::vector<uint32_t> dynamic_offsets;
};

}  // namespace core
}  // namespace mondradiko
