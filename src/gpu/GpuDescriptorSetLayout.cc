/**
 * @file GpuDescriptorSetLayout.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Describes a descriptor set layout.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "gpu/GpuDescriptorSetLayout.h"

#include "log/log.h"

namespace mondradiko {

GpuDescriptorSetLayout::GpuDescriptorSetLayout(GpuInstance* gpu) : gpu(gpu) {}

GpuDescriptorSetLayout::~GpuDescriptorSetLayout() {
  if (set_layout != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(gpu->device, set_layout, nullptr);
}

void GpuDescriptorSetLayout::addCombinedImageSampler(VkSampler sampler) {
  VkDescriptorSetLayoutBinding cis_binding{
      .binding = static_cast<uint32_t>(layout_bindings.size()),
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT};

  layout_bindings.push_back(cis_binding);
  immutable_samplers.push_back(sampler);
}

void GpuDescriptorSetLayout::addDynamicUniformBuffer(uint32_t buffer_size) {
  dynamic_offset_count++;

  VkDescriptorSetLayoutBinding dubo_binding{
      .binding = static_cast<uint32_t>(layout_bindings.size()),
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT};

  layout_bindings.push_back(dubo_binding);
  buffer_sizes.push_back(buffer_size);
}

VkDescriptorSetLayout GpuDescriptorSetLayout::getSetLayout() {
  if (set_layout == VK_NULL_HANDLE) {
    uint32_t sampler_index = 0;

    for(auto& binding : layout_bindings) {
      if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
        binding.pImmutableSamplers = &immutable_samplers[sampler_index];
        sampler_index++;
      }
    }

    VkDescriptorSetLayoutCreateInfo layout_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(layout_bindings.size()),
        .pBindings = layout_bindings.data()};

    if (vkCreateDescriptorSetLayout(gpu->device, &layout_info, nullptr,
                                    &set_layout) != VK_SUCCESS) {
      log_ftl("Failed to create descriptor set layout.");
    }
  }

  return set_layout;
}

}  // namespace mondradiko
