/**
 * @file GpuDescriptorPool.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Allocates descriptors from a dynamic pool.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/gpu/GpuDescriptorPool.h"

#include <vector>

#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

GpuDescriptorPool::GpuDescriptorPool(GpuInstance* gpu) : gpu(gpu) {
  // TODO(marceline-cramer) Dynamic pool recreation using set layouts
  std::vector<VkDescriptorPoolSize> pool_sizes;

  pool_sizes.push_back({.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                        .descriptorCount = 1000});

  pool_sizes.push_back(
      {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = 1000});
  pool_sizes.push_back(
      {.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = 1000});

  pool_sizes.push_back({.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                        .descriptorCount = 1000});

  VkDescriptorPoolCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = 1000,
      .poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
      .pPoolSizes = pool_sizes.data()};

  if (vkCreateDescriptorPool(gpu->device, &createInfo, nullptr,
                             &descriptor_pool) != VK_SUCCESS) {
    log_ftl("Failed to create descriptor pool.");
  }
}

GpuDescriptorPool::~GpuDescriptorPool() {
  if (descriptor_pool != VK_NULL_HANDLE) {
    // Destroy all allocated sets
    reset();
    vkDestroyDescriptorPool(gpu->device, descriptor_pool, nullptr);
  }
}

GpuDescriptorSet* GpuDescriptorPool::allocate(GpuDescriptorSetLayout* layout) {
  VkDescriptorSetLayout vk_set_layout = layout->getSetLayout();
  VkDescriptorSet vk_set;

  VkDescriptorSetAllocateInfo alloc_info{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = descriptor_pool,
      .descriptorSetCount = 1,
      .pSetLayouts = &vk_set_layout};

  if (vkAllocateDescriptorSets(gpu->device, &alloc_info, &vk_set) !=
      VK_SUCCESS) {
    log_ftl("Failed to allocate descriptor set");
  }

  // TODO(marceline-cramer) Dynamic pool resizing

  GpuDescriptorSet* new_set = new GpuDescriptorSet(gpu, layout, vk_set);
  descriptor_sets.emplace(new_set);
  return new_set;
}

void GpuDescriptorPool::reset() {
  vkResetDescriptorPool(gpu->device, descriptor_pool, 0);

  for (const auto& set : descriptor_sets) {
    delete set;
  }

  descriptor_sets.clear();
}

}  // namespace mondradiko
