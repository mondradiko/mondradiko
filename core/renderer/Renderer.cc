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

#include "core/renderer/Renderer.h"

#include "core/displays/DisplayInterface.h"
#include "core/displays/ViewportInterface.h"
#include "core/gpu/GpuBuffer.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/MeshPipeline.h"
#include "log/log.h"

namespace mondradiko {

Renderer::Renderer(DisplayInterface* display, GpuInstance* gpu)
    : display(display), gpu(gpu) {
  log_zone;

  {
    log_zone_named("Create render pass");

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

    VkSubpassDependency swapchain_dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0};

    VkRenderPassCreateInfo compositePassCreateInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = (uint32_t)attachments.size(),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &compositeSubpassDescription,
        .dependencyCount = 1,
        .pDependencies = &swapchain_dependency};

    if (vkCreateRenderPass(gpu->device, &compositePassCreateInfo, nullptr,
                           &composite_pass) != VK_SUCCESS) {
      log_ftl("Failed to create Renderer composite render pass.");
    }
  }

  {
    log_zone_named("Create descriptor set layout");

    viewport_layout = new GpuDescriptorSetLayout(gpu);
    viewport_layout->addDynamicUniformBuffer(sizeof(ViewportUniform));
  }

  {
    log_zone_named("Create frame data");

    // Pipeline two frames
    frames_in_flight.resize(2);

    current_frame = 0;

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

      VkSemaphoreCreateInfo semaphore_info{
          .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

      if (vkCreateSemaphore(gpu->device, &semaphore_info, nullptr,
                            &frame.on_render_finished) != VK_SUCCESS) {
        log_ftl("Failed to create frame render finished semaphore.");
      }

      VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                  .flags = VK_FENCE_CREATE_SIGNALED_BIT};

      if (vkCreateFence(gpu->device, &fenceInfo, nullptr, &frame.is_in_use) !=
          VK_SUCCESS) {
        log_ftl("Failed to create frame fence.");
      }

      frame.descriptor_pool = new GpuDescriptorPool(gpu);

      frame.viewports = new GpuBuffer(
          // TODO(marceline-cramer) Better descriptor management
          gpu, sizeof(ViewportUniform) * 2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VMA_MEMORY_USAGE_CPU_TO_GPU);
    }
  }

  {
    log_zone_named("Create pipelines");

    mesh_pipeline = new MeshPipeline(gpu, viewport_layout, composite_pass, 0);
  }
}

Renderer::~Renderer() {
  log_zone;

  vkDeviceWaitIdle(gpu->device);

  for (auto& frame : frames_in_flight) {
    if (frame.viewports != nullptr) delete frame.viewports;
    if (frame.on_render_finished != VK_NULL_HANDLE)
      vkDestroySemaphore(gpu->device, frame.on_render_finished, nullptr);
    if (frame.is_in_use != VK_NULL_HANDLE)
      vkDestroyFence(gpu->device, frame.is_in_use, nullptr);
    if (frame.descriptor_pool != nullptr) delete frame.descriptor_pool;
  }

  if (mesh_pipeline != nullptr) delete mesh_pipeline;

  if (viewport_layout != nullptr) delete viewport_layout;

  if (composite_pass != VK_NULL_HANDLE)
    vkDestroyRenderPass(gpu->device, composite_pass, nullptr);
}

void Renderer::renderFrame(entt::registry& registry,
                           const AssetPool* asset_pool) {
  log_zone;

  PipelinedFrameData* frame = &frames_in_flight[current_frame];

  {
    log_zone_named("Fence frame");

    vkWaitForFences(gpu->device, 1, &frame->is_in_use, VK_TRUE, UINT64_MAX);
  }

  {
    log_zone_named("Clean up last frame");

    frame->descriptor_pool->reset();
  }

  GpuDescriptorSet* viewport_descriptor;

  {
    log_zone_named("Allocate descriptors");

    viewport_descriptor = frame->descriptor_pool->allocate(viewport_layout);
    viewport_descriptor->updateBuffer(0, frame->viewports);

    mesh_pipeline->allocateDescriptors(registry, asset_pool,
                                       frame->descriptor_pool);
  }

  std::vector<ViewportInterface*> viewports;
  std::vector<VkSemaphore> on_viewport_acquire(0);

  {
    log_zone_named("Acquire viewports");

    display->acquireViewports(&viewports);

    for (uint32_t viewport_index = 0; viewport_index < viewports.size();
         viewport_index++) {
      VkSemaphore on_image_acquire = viewports[viewport_index]->acquire();
      if (on_image_acquire != VK_NULL_HANDLE) {
        on_viewport_acquire.push_back(on_image_acquire);
      }
    }
  }

  {
    log_zone_named("Record command buffers");

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

    vkBeginCommandBuffer(frame->command_buffer, &beginInfo);

    for (uint32_t viewport_index = 0; viewport_index < viewports.size();
         viewport_index++) {
      viewports[viewport_index]->beginRenderPass(frame->command_buffer,
                                                 composite_pass);
      mesh_pipeline->render(registry, asset_pool, frame->command_buffer,
                            viewport_descriptor,
                            viewport_index * sizeof(ViewportUniform));
      vkCmdEndRenderPass(frame->command_buffer);
    }

    vkEndCommandBuffer(frame->command_buffer);
  }

  {
    log_zone_named("Write viewport uniforms");

    std::vector<ViewportUniform> view_uniforms(viewports.size());

    for (uint32_t i = 0; i < viewports.size(); i++) {
      viewports[i]->writeUniform(&view_uniforms.at(i));
    }

    frame->viewports->writeData(view_uniforms.data());
  }

  {
    log_zone_named("Submit to queue");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.waitSemaphoreCount = on_viewport_acquire.size();
    submitInfo.pWaitSemaphores = on_viewport_acquire.data();

    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame->command_buffer;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frame->on_render_finished;

    vkResetFences(gpu->device, 1, &frame->is_in_use);

    if (vkQueueSubmit(gpu->graphics_queue, 1, &submitInfo, frame->is_in_use) !=
        VK_SUCCESS) {
      log_ftl("Failed to submit primary frame command buffer.");
    }
  }

  {
    log_zone_named("Release viewports");

    for (uint32_t viewportIndex = 0; viewportIndex < viewports.size();
         viewportIndex++) {
      viewports[viewportIndex]->release(frame->on_render_finished);
    }
  }

  current_frame++;
  if (current_frame >= frames_in_flight.size()) {
    current_frame = 0;
  }
}

}  // namespace mondradiko
