/**
 * @file GpuDescriptorPool.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Allocates descriptors from a dynamic pool.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_GPU_GPUDESCRIPTORPOOL_H_
#define SRC_GPU_GPUDESCRIPTORPOOL_H_

#include <set>

#include "gpu/GpuDescriptorSet.h"
#include "gpu/GpuDescriptorSetLayout.h"
#include "gpu/GpuInstance.h"

namespace mondradiko {

class GpuDescriptorPool {
 public:
  explicit GpuDescriptorPool(GpuInstance*);
  ~GpuDescriptorPool();

  GpuDescriptorSet* allocate(GpuDescriptorSetLayout*);
  void reset();

 private:
  GpuInstance* gpu;

  VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

  std::set<GpuDescriptorSet*> descriptor_sets;
};

}  // namespace mondradiko

#endif  // SRC_GPU_GPUDESCRIPTORPOOL_H_
