// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/renderer/Renderer.h"

#include "core/cvars/CVarScope.h"
#include "core/displays/DisplayInterface.h"
#include "core/displays/Viewport.h"
#include "core/gpu/GpuDescriptorPool.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuDescriptorSetLayout.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/gpu/GpuVector.h"
#include "core/renderer/OverlayPass.h"
#include "core/renderer/RenderPass.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

void Renderer::initCVars(CVarScope* cvars) {
  CVarScope* renderer = cvars->addChild("renderer");

  OverlayPass::initCVars(renderer);
}

Renderer::Renderer(const CVarScope* cvars, DisplayInterface* display,
                   GpuInstance* gpu)
    : cvars(cvars->getChild("renderer")), display(display), gpu(gpu) {
  log_zone;

  {
    log_zone_named("Create render pass");

    types::vector<VkAttachmentDescription> attachments;

    {
      VkAttachmentDescription swapchain_desc{};
      swapchain_desc.format = display->getSwapchainFormat();
      swapchain_desc.samples = VK_SAMPLE_COUNT_1_BIT;
      swapchain_desc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      swapchain_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      swapchain_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      swapchain_desc.finalLayout = display->getFinalLayout();
      attachments.push_back(swapchain_desc);
    }

    {
      VkAttachmentDescription depth_desc{};
      depth_desc.format = display->getDepthFormat();
      depth_desc.samples = VK_SAMPLE_COUNT_1_BIT;
      depth_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      depth_desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      depth_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      depth_desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
      attachments.push_back(depth_desc);
    }

    VkAttachmentReference swapchain_attachment_reference{};
    swapchain_attachment_reference.attachment = 0;
    swapchain_attachment_reference.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_reference{};
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    types::vector<VkSubpassDescription> subpasses;

    {
      VkSubpassDescription depth_pass{};
      depth_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      depth_pass.colorAttachmentCount = 0;
      depth_pass.pDepthStencilAttachment = &depth_attachment_reference;
      subpasses.push_back(depth_pass);
    }

    {
      VkSubpassDescription forward_pass{};
      forward_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      forward_pass.colorAttachmentCount = 1;
      forward_pass.pColorAttachments = &swapchain_attachment_reference;
      forward_pass.pDepthStencilAttachment = &depth_attachment_reference;
      subpasses.push_back(forward_pass);
    }

    types::vector<VkSubpassDependency> dependencies;

    {
      VkSubpassDependency pre_dep{};
      pre_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
      pre_dep.dstSubpass = 0;
      pre_dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      pre_dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      pre_dep.srcAccessMask = 0;
      pre_dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      dependencies.push_back(pre_dep);
    }

    {
      VkSubpassDependency depth_dep{};
      depth_dep.srcSubpass = 0;
      depth_dep.dstSubpass = 1;
      depth_dep.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
      depth_dep.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
      depth_dep.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      depth_dep.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      dependencies.push_back(depth_dep);
    }

    VkRenderPassCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    ci.attachmentCount = attachments.size();
    ci.pAttachments = attachments.data();
    ci.subpassCount = subpasses.size();
    ci.pSubpasses = subpasses.data();
    ci.dependencyCount = dependencies.size();
    ci.pDependencies = dependencies.data();

    if (vkCreateRenderPass(gpu->device, &ci, nullptr, &render_pass) !=
        VK_SUCCESS) {
      log_ftl("Failed to create Renderer render pass");
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
      VkCommandBufferAllocateInfo alloc_info{};
      alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      alloc_info.commandPool = gpu->command_pool;
      alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      alloc_info.commandBufferCount = 1;

      if (vkAllocateCommandBuffers(gpu->device, &alloc_info,
                                   &frame.command_buffer) != VK_SUCCESS) {
        log_ftl("Failed to allocate frame command buffers.");
      }

      VkSemaphoreCreateInfo semaphore_info{};
      semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

      if (vkCreateSemaphore(gpu->device, &semaphore_info, nullptr,
                            &frame.on_render_finished) != VK_SUCCESS) {
        log_ftl("Failed to create frame render finished semaphore.");
      }

      VkFenceCreateInfo fence_info{};
      fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      if (vkCreateFence(gpu->device, &fence_info, nullptr, &frame.is_in_use) !=
          VK_SUCCESS) {
        log_ftl("Failed to create frame fence.");
      }

      frame.descriptor_pool = new GpuDescriptorPool(gpu);

      frame.viewports = new GpuVector(
          // TODO(marceline-cramer) Better descriptor management
          gpu, sizeof(ViewportUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    }
  }

  {
    log_zone_named("Create error image");

    int error_width = 4;
    int error_height = 4;

    // Magenta-and-green checkerboard
    const uint32_t error_data[] = {
        0xFF00FFFF, 0xFF00FFFF, 0x00FF00FF, 0x00FF00FF,   // NOLINT
        0xFF00FFFF, 0xFF00FFFF, 0x00FF00FF, 0x00FF00FF,   // NOLINT
        0x00FF00FF, 0x00FF00FF, 0xFF00FFFF, 0xFF00FFFF,   // NOLINT
        0x00FF00FF, 0x00FF00FF, 0xFF00FFFF, 0xFF00FFFF};  // NOLINT

    error_image = new GpuImage(
        gpu, VK_FORMAT_R8G8B8A8_UNORM, error_width, error_height, 1,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VMA_MEMORY_USAGE_GPU_ONLY);

    error_image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    transferDataToImage(error_image, error_data);
    error_image->transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }
}

Renderer::~Renderer() {
  log_zone;

  if (error_image != nullptr) delete error_image;

  destroyFrameData();

  if (viewport_layout != nullptr) delete viewport_layout;

  if (render_pass != VK_NULL_HANDLE)
    vkDestroyRenderPass(gpu->device, render_pass, nullptr);
}

void Renderer::addRenderPass(RenderPass* render_pass) {
  log_zone;

  render_pass->createFrameData(frames_in_flight.size());
  render_passes.push_back(render_pass);
}

void Renderer::destroyFrameData() {
  log_zone;

  vkDeviceWaitIdle(gpu->device);

  for (auto& render_pass : render_passes) {
    render_pass->destroyFrameData();
  }

  for (auto& frame : frames_in_flight) {
    if (frame.viewports != nullptr) delete frame.viewports;
    if (frame.on_render_finished != VK_NULL_HANDLE)
      vkDestroySemaphore(gpu->device, frame.on_render_finished, nullptr);
    if (frame.is_in_use != VK_NULL_HANDLE)
      vkDestroyFence(gpu->device, frame.is_in_use, nullptr);
    if (frame.descriptor_pool != nullptr) delete frame.descriptor_pool;
  }

  render_passes.clear();
  frames_in_flight.clear();
}

void Renderer::transferDataToBuffer(GpuBuffer* dst, size_t offset,
                                    const void* src, size_t size) {
  log_zone;

  // TODO(marceline-cramer) Yikes

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocation_info;

  if (vmaCreateBuffer(gpu->allocator, &bufferInfo, &allocationCreateInfo,
                      &buffer, &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer.");
  }

  memcpy(allocation_info.pMappedData, src, allocation_info.size);

  VkCommandBuffer command_buffer = gpu->beginSingleTimeCommands();
  VkBufferCopy buffer_copy{};
  buffer_copy.srcOffset = 0;
  buffer_copy.dstOffset = offset;
  buffer_copy.size = size;
  vkCmdCopyBuffer(command_buffer, buffer, dst->getBuffer(), 1, &buffer_copy);
  gpu->endSingleTimeCommands(command_buffer);

  // TODO(marceline-cramer) Dedicated transfer queue
  {  // TODO(marceline-cramer) Also yikes
    log_zone_named("Pipeline stall");
    vkQueueWaitIdle(gpu->graphics_queue);
  }
  vmaDestroyBuffer(gpu->allocator, buffer, allocation);
}

void Renderer::transferDataToImage(GpuImage* dst, const void* src) {
  log_zone;

  // TODO(marceline-cramer) Yikes

  size_t size = dst->getSize();

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocation_info;

  if (vmaCreateBuffer(gpu->allocator, &bufferInfo, &allocationCreateInfo,
                      &buffer, &allocation, &allocation_info) != VK_SUCCESS) {
    log_ftl("Failed to allocate Vulkan buffer");
  }

  memcpy(allocation_info.pMappedData, src, allocation_info.size);

  VkCommandBuffer commandBuffer = gpu->beginSingleTimeCommands();

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  VkImageSubresourceLayers imageSubresource{};
  imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubresource.mipLevel = 0;
  imageSubresource.baseArrayLayer = 0;
  imageSubresource.layerCount = 1;
  region.imageSubresource = imageSubresource;

  VkOffset3D image_offset{};
  image_offset.x = 0;
  image_offset.y = 0;
  image_offset.z = 0;
  region.imageOffset = image_offset;

  VkExtent3D image_extent{};
  image_extent.width = dst->getWidth();
  image_extent.height = dst->getHeight();
  image_extent.depth = 1;
  region.imageExtent = image_extent;

  vkCmdCopyBufferToImage(commandBuffer, buffer, dst->getImage(),
                         dst->getLayout(), 1, &region);

  gpu->endSingleTimeCommands(commandBuffer);

  {  // TODO(marceline-cramer) Yup, this is still bad
    log_zone_named("Pipeline stall");
    vkQueueWaitIdle(gpu->graphics_queue);
  }
  vmaDestroyBuffer(gpu->allocator, buffer, allocation);
}

void Renderer::renderFrame() {
  log_zone;

  current_frame++;
  if (current_frame >= frames_in_flight.size()) {
    current_frame = 0;
  }

  auto& frame = frames_in_flight[current_frame];

  {
    log_zone_named("Fence frame");

    vkWaitForFences(gpu->device, 1, &frame.is_in_use, VK_TRUE, UINT64_MAX);
  }

  {
    log_zone_named("Clean up last frame");

    frame.descriptor_pool->reset();

    for (auto& phase : frame.phases) {
      phase.clear();
    }
  }

  GpuDescriptorSet* viewport_descriptor;

  {
    log_zone_named("Begin frame");

    viewport_descriptor = frame.descriptor_pool->allocate(viewport_layout);
    viewport_descriptor->updateDynamicBuffer(0, frame.viewports);

    for (auto& render_pass : render_passes) {
      render_pass->beginFrame(current_frame, frame.descriptor_pool);
    }
  }

  {
    log_zone_named("Begin command buffers");

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(frame.command_buffer, &begin_info);
  }

  const size_t viewport_phase = static_cast<size_t>(RenderPhase::Depth);
  size_t phase_idx = 0;

  {
    log_zone_named("Render pre-viewport");

    for (phase_idx = 0; phase_idx < viewport_phase; phase_idx++) {
      auto& phase_passes = frame.phases[phase_idx];
      RenderPhase phase = static_cast<RenderPhase>(phase_idx);
      for (auto& pass : phase_passes) {
        pass->render(phase, frame.command_buffer);
      }
    }
  }

  types::vector<Viewport*> viewports;
  types::vector<VkSemaphore> on_viewport_acquire(0);
  bool viewports_require_signal = false;

  {
    log_zone_named("Acquire viewports");

    display->acquireViewports(&viewports);

    for (uint32_t viewport_index = 0; viewport_index < viewports.size();
         viewport_index++) {
      VkSemaphore on_image_acquire = viewports[viewport_index]->acquire();
      if (on_image_acquire != VK_NULL_HANDLE) {
        on_viewport_acquire.push_back(on_image_acquire);
      }

      if (viewports[viewport_index]->isSignalRequired()) {
        viewports_require_signal = true;
      }
    }
  }

  {
    log_zone_named("Render viewports");

    for (uint32_t viewport_index = 0; viewport_index < viewports.size();
         viewport_index++) {
      viewport_descriptor->updateDynamicOffset(
          0, viewport_index * sizeof(ViewportUniform));
      viewports[viewport_index]->beginRenderPass(frame.command_buffer,
                                                 render_pass);

      for (phase_idx = viewport_phase; phase_idx < frame.phases.size();
           phase_idx++) {
        if (phase_idx != viewport_phase) {
          vkCmdNextSubpass(frame.command_buffer, VK_SUBPASS_CONTENTS_INLINE);
        }

        auto& phase_passes = frame.phases[phase_idx];
        RenderPhase phase = static_cast<RenderPhase>(phase_idx);
        for (auto& pass : phase_passes) {
          pass->renderViewport(phase, frame.command_buffer,
                               viewport_descriptor);
        }
      }

      vkCmdEndRenderPass(frame.command_buffer);
    }

    vkEndCommandBuffer(frame.command_buffer);
  }

  {
    log_zone_named("Write viewport uniforms");

    for (uint32_t i = 0; i < viewports.size(); i++) {
      ViewportUniform uniform;
      viewports[i]->writeUniform(&uniform);
      frame.viewports->writeElement(i, uniform);
    }
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
    submitInfo.pCommandBuffers = &frame.command_buffer;

    if (viewports_require_signal) {
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = &frame.on_render_finished;
    } else {
      submitInfo.signalSemaphoreCount = 0;
      submitInfo.pSignalSemaphores = nullptr;
    }

    vkResetFences(gpu->device, 1, &frame.is_in_use);

    if (vkQueueSubmit(gpu->graphics_queue, 1, &submitInfo, frame.is_in_use) !=
        VK_SUCCESS) {
      log_ftl("Failed to submit primary frame command buffer.");
    }
  }

  {
    log_zone_named("Release viewports");

    for (uint32_t viewportIndex = 0; viewportIndex < viewports.size();
         viewportIndex++) {
      viewports[viewportIndex]->release(frame.on_render_finished);
    }
  }
}

void Renderer::addPassToPhase(RenderPhase phase, RenderPass* pass) {
  auto& frame = frames_in_flight[current_frame];
  frame.phases[static_cast<size_t>(phase)].push_back(pass);
}

}  // namespace core
}  // namespace mondradiko
