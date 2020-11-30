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

void GpuDescriptorSetLayout::addDynamicUniformBuffer() {
  VkDescriptorSetLayoutBinding dubo_binding{
      .binding = static_cast<uint32_t>(layout_bindings.size()),
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .descriptorCount = 1,
      // TODO(marceline-cramer) Reflection w/ stage flags
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT};

  layout_bindings.push_back(dubo_binding);
}

VkDescriptorSetLayout GpuDescriptorSetLayout::getSetLayout() {
  if (set_layout == VK_NULL_HANDLE) {
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
