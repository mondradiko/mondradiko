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

#include "gpu/GpuBuffer.h"
#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class GpuDescriptorSet {
 public:
  GpuDescriptorSet(GpuInstance*, VkDescriptorSet);
  ~GpuDescriptorSet();

  void updateBuffer(uint32_t, GpuBuffer*);

  VkDescriptorSet descriptor_set;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko

#endif  // SRC_GPU_GPUDESCRIPTORSET_H_
