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

#ifndef SRC_GPU_GPUDESCRIPTORSET_H_
#define SRC_GPU_GPUDESCRIPTORSET_H_

#include <vector>

#include "gpu/GpuBuffer.h"
#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

// TODO(marceline-cramer) Named descriptors
// TODO(marceline-cramer) Reflection

class GpuDescriptorSet {
 public:
  GpuDescriptorSet(GpuInstance*, VkDescriptorSet, uint32_t);
  ~GpuDescriptorSet();

  void updateBuffer(uint32_t, GpuBuffer*);

  void updateDynamicOffset(uint32_t, uint32_t);

  void cmdBind(VkCommandBuffer, VkPipelineLayout, uint32_t);

 private:
  GpuInstance* gpu;

  VkDescriptorSet descriptor_set;
  std::vector<uint32_t> dynamic_offsets;
};

}  // namespace mondradiko

#endif  // SRC_GPU_GPUDESCRIPTORSET_H_
