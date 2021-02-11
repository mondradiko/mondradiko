// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuDescriptorSetLayout.h"

#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

GpuDescriptorSetLayout::GpuDescriptorSetLayout(GpuInstance* gpu) : gpu(gpu) {}

GpuDescriptorSetLayout::~GpuDescriptorSetLayout() {
  if (set_layout != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(gpu->device, set_layout, nullptr);
}

void GpuDescriptorSetLayout::addCombinedImageSampler(VkSampler sampler) {
  VkDescriptorSetLayoutBinding cis_binding{};
  cis_binding.binding = static_cast<uint32_t>(layout_bindings.size());
  cis_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  cis_binding.descriptorCount = 1;
  cis_binding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  layout_bindings.push_back(cis_binding);
  immutable_samplers.push_back(sampler);
}

void GpuDescriptorSetLayout::addStorageBuffer(uint32_t element_size) {
  VkDescriptorSetLayoutBinding storage_binding{};
  storage_binding.binding = static_cast<uint32_t>(layout_bindings.size());
  storage_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  storage_binding.descriptorCount = 1;
  storage_binding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  layout_bindings.push_back(storage_binding);
  buffer_sizes.push_back(element_size);
}

void GpuDescriptorSetLayout::addDynamicUniformBuffer(uint32_t buffer_size) {
  dynamic_offset_count++;

  VkDescriptorSetLayoutBinding dubo_binding{};
  dubo_binding.binding = static_cast<uint32_t>(layout_bindings.size());
  dubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  dubo_binding.descriptorCount = 1;
  dubo_binding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  layout_bindings.push_back(dubo_binding);
  buffer_sizes.push_back(buffer_size);
}

VkDescriptorSetLayout GpuDescriptorSetLayout::getSetLayout() {
  if (set_layout == VK_NULL_HANDLE) {
    uint32_t sampler_index = 0;

    for (auto& binding : layout_bindings) {
      if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
        binding.pImmutableSamplers = &immutable_samplers[sampler_index];
        sampler_index++;
      }
    }

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = static_cast<uint32_t>(layout_bindings.size());
    layout_info.pBindings = layout_bindings.data();

    if (vkCreateDescriptorSetLayout(gpu->device, &layout_info, nullptr,
                                    &set_layout) != VK_SUCCESS) {
      log_ftl("Failed to create descriptor set layout.");
    }
  }

  return set_layout;
}

}  // namespace mondradiko
