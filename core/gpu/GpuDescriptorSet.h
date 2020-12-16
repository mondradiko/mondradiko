/**
 * @file GpuDescriptorSet.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief An instance of a bindable GPU descriptor set.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/common/api_headers.h"

namespace mondradiko {

// TODO(marceline-cramer) Named descriptors
// TODO(marceline-cramer) Reflection

// Forward declarations
class GpuBuffer;
class GpuDescriptorSetLayout;
class GpuImage;
class GpuInstance;

class GpuDescriptorSet {
 public:
  GpuDescriptorSet(GpuInstance*, GpuDescriptorSetLayout*, VkDescriptorSet);
  ~GpuDescriptorSet();

  void updateBuffer(uint32_t, GpuBuffer*);
  void updateImage(uint32_t, GpuImage*);

  void updateDynamicOffset(uint32_t, uint32_t);

  void cmdBind(VkCommandBuffer, VkPipelineLayout, uint32_t);

 private:
  GpuInstance* gpu;

  GpuDescriptorSetLayout* set_layout;
  VkDescriptorSet descriptor_set;
  std::vector<uint32_t> dynamic_offsets;
};

}  // namespace mondradiko
