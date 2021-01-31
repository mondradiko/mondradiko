// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuDescriptorSet.h"

#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuVector.h"

namespace mondradiko {

GpuDescriptorSet::GpuDescriptorSet(GpuInstance* gpu,
                                   GpuDescriptorSetLayout* set_layout,
                                   VkDescriptorSet descriptor_set)
    : gpu(gpu), set_layout(set_layout), descriptor_set(descriptor_set) {
  uint32_t dynamic_offset_count = set_layout->getDynamicOffsetCount();
  dynamic_offset_granularity.resize(dynamic_offset_count);
  dynamic_offsets.resize(dynamic_offset_count);
}

GpuDescriptorSet::~GpuDescriptorSet() {}

void GpuDescriptorSet::updateBuffer(uint32_t binding, GpuBuffer* buffer) {
  VkDescriptorBufferInfo buffer_info{
      .buffer = buffer->getBuffer(),
      .offset = 0,
      .range = set_layout->getBufferSize(binding)};

  VkWriteDescriptorSet descriptor_writes{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptor_set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = &buffer_info};

  vkUpdateDescriptorSets(gpu->device, 1, &descriptor_writes, 0, nullptr);
}

void GpuDescriptorSet::updateDynamicBuffer(uint32_t binding,
                                           GpuVector* buffer) {
  VkDescriptorBufferInfo buffer_info{
      .buffer = buffer->getBuffer(),
      .offset = 0,
      .range = set_layout->getBufferSize(binding)};

  VkWriteDescriptorSet descriptor_writes{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptor_set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      .pBufferInfo = &buffer_info};

  vkUpdateDescriptorSets(gpu->device, 1, &descriptor_writes, 0, nullptr);
}

void GpuDescriptorSet::updateStorageBuffer(uint32_t binding,
                                           const GpuBuffer* buffer) {
  VkDescriptorBufferInfo buffer_info{.buffer = buffer->getBuffer(),
                                     .offset = 0,
                                     .range = buffer->getBufferSize()};

  VkWriteDescriptorSet descriptor_writes{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptor_set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .pBufferInfo = &buffer_info};

  vkUpdateDescriptorSets(gpu->device, 1, &descriptor_writes, 0, nullptr);
}

void GpuDescriptorSet::updateImage(uint32_t binding, const GpuImage* image) {
  VkDescriptorImageInfo image_info{.imageView = image->view,
                                   .imageLayout = image->layout};

  VkWriteDescriptorSet descriptor_writes{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptor_set,
      .dstBinding = binding,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = &image_info};

  vkUpdateDescriptorSets(gpu->device, 1, &descriptor_writes, 0, nullptr);
}

void GpuDescriptorSet::updateDynamicOffset(uint32_t binding, uint32_t offset) {
  dynamic_offsets[binding] = offset * dynamic_offset_granularity[binding];
}

void GpuDescriptorSet::cmdBind(VkCommandBuffer command_buffer,
                               VkPipelineLayout pipeline_layout,
                               uint32_t binding) const {
  vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline_layout, binding, 1, &descriptor_set,
                          static_cast<uint32_t>(dynamic_offsets.size()),
                          dynamic_offsets.data());
}

}  // namespace mondradiko
