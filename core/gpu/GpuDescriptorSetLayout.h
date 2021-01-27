/**
 * @file GpuDescriptorSetLayout.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Describes a descriptor set layout.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "lib/include/vulkan_headers.h"

namespace mondradiko {

// Forward declarations
class GpuInstance;

class GpuDescriptorSetLayout {
 public:
  explicit GpuDescriptorSetLayout(GpuInstance*);
  ~GpuDescriptorSetLayout();

  // TODO(marceline-cramer) SPIR-V reflection w/ stage flags
  // TODO(marceline-cramer) Implement these as needed
  // void addSampler();
  void addCombinedImageSampler(VkSampler);
  // void addSampledImage();
  // void addStorageImage();
  // void addUniformTexelBuffer();
  // void addStorageTexelBuffer();
  // void addUniformBuffer();
  void addStorageBuffer(uint32_t);
  void addDynamicUniformBuffer(uint32_t);
  // void addDynamicStorageBuffer();
  // void addInputAttachment();

  VkDescriptorSetLayout getSetLayout();
  uint32_t getBufferSize(uint32_t index) { return buffer_sizes[index]; }
  uint32_t getDynamicOffsetCount() { return dynamic_offset_count; }

 private:
  GpuInstance* gpu;

  std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
  std::vector<VkSampler> immutable_samplers;
  std::vector<uint32_t> buffer_sizes;
  uint32_t dynamic_offset_count = 0;

  VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
};

}  // namespace mondradiko
