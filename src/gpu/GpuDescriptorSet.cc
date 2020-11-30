/**
 * @file GpuDescriptorSet.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief An instance of a bindable GPU descriptor set.
 * @date 2020-11-30
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "gpu/GpuDescriptorSet.h"

namespace mondradiko {

GpuDescriptorSet::GpuDescriptorSet(GpuInstance* gpu,
                                   VkDescriptorSet descriptor_set)
    : gpu(gpu), descriptor_set(descriptor_set) {}

GpuDescriptorSet::~GpuDescriptorSet() {}

void GpuDescriptorSet::updateBuffer(uint32_t binding, GpuBuffer* buffer) {
  VkDescriptorBufferInfo buffer_info{
      .buffer = buffer->buffer, .offset = 0, .range = buffer->buffer_size};

  VkWriteDescriptorSet descriptor_writes{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptor_set,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .pBufferInfo = &buffer_info};

  vkUpdateDescriptorSets(gpu->device, 1, &descriptor_writes, 0, nullptr);
}

}  // namespace mondradiko
