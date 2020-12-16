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

#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations
class GpuDescriptorSet;
class GpuDescriptorSetLayout;
class GpuInstance;

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
