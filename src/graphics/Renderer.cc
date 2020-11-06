/**
 * @file Renderer.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains all Pipelines and performs frame rendering operations.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "graphics/Renderer.h"

#include "log/log.h"

namespace mondradiko {

Renderer::Renderer(VulkanInstance* _vulkanInstance, PlayerSession* _session) {
  log_dbg("Creating renderer.");

  vulkanInstance = _vulkanInstance;
  session = _session;

  findSwapchainFormat();
  createRenderPasses();

  if (!session->createViewports(&viewports, swapchainFormat, compositePass)) {
    log_ftl("Failed to create Renderer viewports.");
  }

  createDescriptorSetLayout();
  createPipelineLayout();
  createFrameData();
  createPipelines();
}

Renderer::~Renderer() {
  log_dbg("Destroying renderer.");

  vkDeviceWaitIdle(vulkanInstance->device);

  for (auto& frame : framesInFlight) {
    if (frame.viewports != nullptr) delete frame.viewports;
    if (frame.isInUse != VK_NULL_HANDLE)
      vkDestroyFence(vulkanInstance->device, frame.isInUse, nullptr);
  }

  if (meshPipeline != nullptr) delete meshPipeline;
  if (main_descriptor_layout != VK_NULL_HANDLE)
    vkDestroyDescriptorSetLayout(vulkanInstance->device, main_descriptor_layout,
                                 nullptr);
  if (main_pipeline_layout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(vulkanInstance->device, main_pipeline_layout,
                            nullptr);

  for (Viewport* viewport : viewports) {
    delete viewport;
  }

  if (compositePass != VK_NULL_HANDLE)
    vkDestroyRenderPass(vulkanInstance->device, compositePass, nullptr);
}

void Renderer::renderFrame() {
  for (uint32_t viewportIndex = 0; viewportIndex < viewports.size();
       viewportIndex++) {
    viewports[viewportIndex]->acquireSwapchainImage();
  }

  if (framesInFlight[currentFrame].submitted) {
    vkWaitForFences(vulkanInstance->device, 1,
                    &framesInFlight[currentFrame].isInUse, VK_TRUE, UINT64_MAX);
    framesInFlight[currentFrame].submitted = false;
  }

  if (++currentFrame >= framesInFlight.size()) {
    currentFrame = 0;
  }

  PipelinedFrameData* frame = &framesInFlight[currentFrame];

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

  vkBeginCommandBuffer(frame->commandBuffer, &beginInfo);

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

  vkUpdateDescriptorSets(vulkanInstance->device, 1, &descriptor_writes, 0,
                         nullptr);

  meshPipeline->updateDescriptors(frame->descriptors);

  vkCmdBindDescriptorSets(frame->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          main_pipeline_layout, 0, 1, &frame->descriptors, 0,
                          nullptr);

  for (uint32_t viewportIndex = 0; viewportIndex < viewports.size();
       viewportIndex++) {
    FramePushConstant push_constant{.view_index = viewportIndex};

    vkCmdPushConstants(
        frame->commandBuffer, main_pipeline_layout,
        VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT, 0,
        sizeof(FramePushConstant), &push_constant);

    viewports[viewportIndex]->beginRenderPass(frame->commandBuffer,
                                              compositePass);
    viewports[viewportIndex]->setCommandViewport(frame->commandBuffer);
    meshPipeline->render(frame->commandBuffer);
    vkCmdEndRenderPass(frame->commandBuffer);
  }
}

void Renderer::finishRender(
    std::vector<XrView>* views,
    std::vector<XrCompositionLayerProjectionView>* projectionViews) {
  std::vector<ViewportUniform> view_uniforms(viewports.size());

  for (uint32_t i = 0; i < viewports.size(); i++) {
    viewports[i]->updateView(views->at(i), &projectionViews->at(i),
                             &view_uniforms.at(i));
  }

  PipelinedFrameData* frame = &framesInFlight[currentFrame];
  frame->viewports->writeData(view_uniforms.data());

  vkEndCommandBuffer(frame->commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 0;
  submitInfo.pWaitSemaphores = nullptr;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &frame->commandBuffer;

  submitInfo.signalSemaphoreCount = 0;
  submitInfo.pSignalSemaphores = nullptr;

  vkResetFences(vulkanInstance->device, 1, &frame->isInUse);

  if (vkQueueSubmit(vulkanInstance->graphicsQueue, 1, &submitInfo,
                    frame->isInUse) != VK_SUCCESS) {
    log_ftl("Failed to submit primary frame command buffer.");
  }

  frame->submitted = true;

  for (uint32_t viewportIndex = 0; viewportIndex < viewports.size();
       viewportIndex++) {
    viewports[viewportIndex]->releaseSwapchainImage();
  }
}

void Renderer::findSwapchainFormat() {
  std::vector<VkFormat> formatOptions;
  session->enumerateSwapchainFormats(&formatOptions);

  std::vector<VkFormat> formatCandidates = {VK_FORMAT_R8G8B8A8_SRGB,
                                            VK_FORMAT_R8G8B8A8_UNORM};

  if (!vulkanInstance->findFormatFromOptions(&formatOptions, &formatCandidates,
                                             &swapchainFormat)) {
    log_ftl("Failed to find suitable swapchain format.");
  }
}

void Renderer::createRenderPasses() {
  VkAttachmentDescription swapchainAttachmentDescription{
      .format = swapchainFormat,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

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

  if (vkCreateRenderPass(vulkanInstance->device, &compositePassCreateInfo,
                         nullptr, &compositePass) != VK_SUCCESS) {
    log_ftl("Failed to create Renderer composite render pass.");
  }
}

void Renderer::createDescriptorSetLayout() {
  std::vector<VkDescriptorSetLayoutBinding> bindings;

  bindings.push_back(VkDescriptorSetLayoutBinding{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = static_cast<uint32_t>(viewports.size()),
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

  if (vkCreateDescriptorSetLayout(vulkanInstance->device, &layoutInfo, nullptr,
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

  if (vkCreatePipelineLayout(vulkanInstance->device, &layoutInfo, nullptr,
                             &main_pipeline_layout) != VK_SUCCESS) {
    log_ftl("Failed to create Renderer pipeline layout.");
  }
}

void Renderer::createFrameData() {
  // Pipeline two frames
  framesInFlight.resize(2);

  for (auto& frame : framesInFlight) {
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vulkanInstance->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1};

    if (vkAllocateCommandBuffers(vulkanInstance->device, &allocInfo,
                                 &frame.commandBuffer) != VK_SUCCESS) {
      log_ftl("Failed to allocate frame command buffers.");
    }

    VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

    if (vkCreateFence(vulkanInstance->device, &fenceInfo, nullptr,
                      &frame.isInUse) != VK_SUCCESS) {
      log_ftl("Failed to create frame fence.");
    }

    frame.submitted = false;

    VkDescriptorSetAllocateInfo descriptorInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vulkanInstance->descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &main_descriptor_layout};

    if (vkAllocateDescriptorSets(vulkanInstance->device, &descriptorInfo,
                                 &frame.descriptors) != VK_SUCCESS) {
      log_ftl("Failed to allocate frame descriptors.");
    }

    frame.viewports = new VulkanBuffer(
        vulkanInstance, sizeof(ViewportUniform) * viewports.size(),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
  }
}

void Renderer::createPipelines() {
  meshPipeline =
      new MeshPipeline(vulkanInstance, main_pipeline_layout, compositePass, 0);
}

}  // namespace mondradiko
