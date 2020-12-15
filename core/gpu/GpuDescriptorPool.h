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

#pragma once

#include <set>

#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"

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
