/**
 * @file Renderer.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all Pipelines and performs frame rendering operations.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "renderer/Renderer.h"

#include "log/log.h"

namespace mondradiko {

Renderer::Renderer(DisplayInterface* display, GpuInstance* gpu)
    : display(display), gpu(gpu) {
  log_dbg("Creating renderer.");

  createRenderPasses();
  createDescriptorSetLayout();
  createPipelineLayout();
  createFrameData();
  createPipelines();
}

Renderer::~Renderer() {
  log_dbg("Destroying renderer.");

  vkDeviceWaitIdle(gpu->device);

  for (auto& frame : frames_in_flight) {
    if (frame.viewports != nullptr) delete frame.viewports;
    if (frame.is_in_use != VK_NULL_HANDLE)
      vkDestroyFence(gpu->device, frame.is_in_use, nullptr);
  }

  if (mesh_pipeline != nullptr) delete mesh_pipeline;
  if (main_descriptor_layout != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(gpu->device, main_descriptor_layout, nullptr);
  if (main_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(gpu->device, main_pipeline_layout, nullptr);

  if (composite_pass != VK_NULL_HANDLE)
    vkDestroyRenderPass(gpu->device, composite_pass, nullptr);
}

void Renderer::renderFrame() {
  if (frames_in_flight[current_frame].submitted) {
    vkWaitForFences(gpu->device, 1, &frames_in_flight[current_frame].is_in_use,
                    VK_TRUE, UINT64_MAX);
    frames_in_flight[current_frame].submitted = false;
  }

  if (++current_frame >= frames_in_flight.size()) {
    current_frame = 0;
  }

  PipelinedFrameData* frame = &frames_in_flight[current_frame];

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

  vkBeginCommandBuffer(frame->command_buffer, &beginInfo);

  std::vector<VkDescriptorBufferInfo> viewport_buffer_infos;

  viewport_buffer_infos.push_back(
      VkDescriptorBufferInfo{.buffer = frame->viewports->buffer,
                             .offset = 0,
                             .range = sizeof(ViewportUniform)});

  viewport_buffer_infos.push_back(
      VkDescriptorBufferInfo{.buffer = frame->viewports->buffer,
                             .offset = sizeof(ViewportUniform),
                             .range = sizeof(ViewportUniform)});

  VkWriteDescriptorSet descriptor_writes{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = frame->descriptors,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = static_cast<uint32_t>(viewport_buffer_infos.size()),
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .pBufferInfo = viewport_buffer_infos.data()};

  vkUpdateDescriptorSets(gpu->device, 1, &descriptor_writes, 0, nullptr);

  mesh_pipeline->updateDescriptors(frame->descriptors);

  vkCmdBindDescriptorSets(frame->command_buffer,
                          VK_PIPELINE_BIND_POINT_GRAPHICS, main_pipeline_layout,
                          0, 1, &frame->descriptors, 0, nullptr);

  std::vector<ViewportInterface*> viewports;
  display->acquireViewports(&viewports);

  for (uint32_t viewport_index = 0; viewport_index < viewports.size();
       viewport_index++) {
    FramePushConstant push_constant{.view_index = viewport_index};

    vkCmdPushConstants(
        frame->command_buffer, main_pipeline_layout,
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0,
        sizeof(FramePushConstant), &push_constant);

    viewports[viewport_index]->acquire();
    viewports[viewport_index]->beginRenderPass(frame->command_buffer,
                                               composite_pass);
    mesh_pipeline->render(frame->command_buffer);
    vkCmdEndRenderPass(frame->command_buffer);
  }

  std::vector<ViewportUniform> view_uniforms(viewports.size());

  for (uint32_t i = 0; i < viewports.size(); i++) {
    viewports[i]->writeUniform(&view_uniforms.at(i));
  }

  frame->viewports->writeData(view_uniforms.data());

  vkEndCommandBuffer(frame->command_buffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 0;
  submitInfo.pWaitSemaphores = nullptr;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &frame->command_buffer;

  submitInfo.signalSemaphoreCount = 0;
  submitInfo.pSignalSemaphores = nullptr;

  vkResetFences(gpu->device, 1, &frame->is_in_use);

  if (vkQueueSubmit(gpu->graphics_queue, 1, &submitInfo, frame->is_in_use) !=
      VK_SUCCESS) {
    log_ftl("Failed to submit primary frame command buffer.");
  }

  frame->submitted = true;

  for (uint32_t viewportIndex = 0; viewportIndex < viewports.size();
       viewportIndex++) {
    viewports[viewportIndex]->release();
  }
}

void Renderer::createRenderPasses() {
  VkAttachmentDescription swapchainAttachmentDescription{
      .format = display->getSwapchainFormat(),
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = display->getFinalLayout()};

  VkAttachmentReference swapchainTargetReference{
      .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  std::vector<VkAttachmentDescription> attachments = {
      swapchainAttachmentDescription};

  VkSubpassDescription compositeSubpassDescription{
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .colorAttachmentCount = 1,
      .pColorAttachments = &swapchainTargetReference};

  VkRenderPassCreateInfo compositePassCreateInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .attachmentCount = (uint32_t)attachments.size(),
      .pAttachments = attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &compositeSubpassDescription};

  if (vkCreateRenderPass(gpu->device, &compositePassCreateInfo, nullptr,
                         &composite_pass) != VK_SUCCESS) {
    log_ftl("Failed to create Renderer composite render pass.");
  }
}

void Renderer::createDescriptorSetLayout() {
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  bindings.push_back(VkDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      // TODO(marceline-cramer) Better descriptor management
      .descriptorCount = 2,
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT});

  bindings.push_back(VkDescriptorSetLayoutBinding{
      .binding = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 128,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT});

  bindings.push_back(VkDescriptorSetLayoutBinding{
      .binding = 2,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT});

  VkDescriptorSetLayoutCreateInfo layoutInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<uint32_t>(bindings.size()),
      .pBindings = bindings.data()};

  if (vkCreateDescriptorSetLayout(gpu->device, &layoutInfo, nullptr,
                                  &main_descriptor_layout) != VK_SUCCESS) {
    log_ftl("Failed to create Renderer main descriptor set layout.");
  }
}

void Renderer::createPipelineLayout() {
  VkPushConstantRange constantRange{
      .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      .offset = 0,
      .size = sizeof(FramePushConstant)};

  VkPipelineLayoutCreateInfo layoutInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &main_descriptor_layout,
      .pushConstantRangeCount = 1,
      .pPushConstantRanges = &constantRange};

  if (vkCreatePipelineLayout(gpu->device, &layoutInfo, nullptr,
                             &main_pipeline_layout) != VK_SUCCESS) {
    log_ftl("Failed to create Renderer pipeline layout.");
  }
}

void Renderer::createFrameData() {
  // Pipeline two frames
  frames_in_flight.resize(2);

  for (auto& frame : frames_in_flight) {
    VkCommandBufferAllocateInfo alloc_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = gpu->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};

    if (vkAllocateCommandBuffers(gpu->device, &alloc_info,
                                 &frame.command_buffer) != VK_SUCCESS) {
      log_ftl("Failed to allocate frame command buffers.");
    }

    VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    if (vkCreateFence(gpu->device, &fenceInfo, nullptr, &frame.is_in_use) !=
        VK_SUCCESS) {
      log_ftl("Failed to create frame fence.");
    }

    frame.submitted = false;

    VkDescriptorSetAllocateInfo set_info{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = gpu->descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &main_descriptor_layout};

    if (vkAllocateDescriptorSets(gpu->device, &set_info, &frame.descriptors) !=
        VK_SUCCESS) {
      log_ftl("Failed to allocate frame descriptors.");
    }

    frame.viewports = new GpuBuffer(
        // TODO(marceline-cramer) Better descriptor management
        gpu, sizeof(ViewportUniform) * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_MEMORY_USAGE_CPU_TO_GPU);
  }
}

void Renderer::createPipelines() {
  mesh_pipeline =
      new MeshPipeline(gpu, main_pipeline_layout, composite_pass, 0);
}

}  // namespace mondradiko
