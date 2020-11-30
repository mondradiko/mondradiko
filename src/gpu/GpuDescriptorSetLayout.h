/**
 * @file GpuDescriptorSetLayout.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Describes a descriptor set layout.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_GPU_GPUDESCRIPTORSETLAYOUT_H_
#define SRC_GPU_GPUDESCRIPTORSETLAYOUT_H_

#include <vector>

#include "gpu/GpuInstance.h"

namespace mondradiko {

class GpuDescriptorSetLayout {
 public:
  GpuDescriptorSetLayout(GpuInstance*);
  ~GpuDescriptorSetLayout();

  // TODO(marceline-cramer) SPIR-V reflection w/ stage flags
  // TODO(marceline-cramer) Implement these as needed
  // void addSampler();
  // void addCombinedImageSampler();
  // void addSampledImage();
  // void addStorageImage();
  // void addUniformTexelBuffer();
  // void addStorageTexelBuffer();
  void addDynamicUniformBuffer();
  // void addDynamicStorageBuffer();
  // void addInputAttachment();

  VkDescriptorSetLayout getSetLayout();

 private:
  GpuInstance* gpu;

  std::vector<VkDescriptorSetLayoutBinding> layout_bindings;

  VkDescriptorSetLayout set_layout = VK_NULL_HANDLE;
};

}  // namespace mondradiko

#endif  // SRC_GPU_GPUDESCRIPTORSETLAYOUT_H_
