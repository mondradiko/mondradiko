/**
 * @file GpuDescriptorSet.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief An instance of a bindable GPU descriptor set.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/common/vulkan_headers.h"

namespace mondradiko {

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
  void updateImage(uint32_t, GpuImage*);

  void updateDynamicOffset(uint32_t, uint32_t);

  void cmdBind(VkCommandBuffer, VkPipelineLayout, uint32_t);

 private:
  GpuInstance* gpu;

  // Only GpuDescriptorPools are allowed to destroy sets
  friend class GpuDescriptorPool;
  ~GpuDescriptorSet();

  GpuDescriptorSetLayout* set_layout;
  VkDescriptorSet descriptor_set;
  std::vector<uint32_t> dynamic_offset_granularity;
  std::vector<uint32_t> dynamic_offsets;
};

}  // namespace mondradiko
