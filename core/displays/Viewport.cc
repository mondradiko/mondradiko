// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/displays/Viewport.h"

#include "core/displays/DisplayInterface.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"

namespace mondradiko {

void Viewport::beginRenderPass(VkCommandBuffer command_buffer,
                               VkRenderPass render_pass) {
  log_zone;

  std::array<VkClearValue, 2> clear_values;

  clear_values[0].color = {0.2, 0.0, 0.0, 1.0};
  clear_values[1].depthStencil = {1.0f};

  VkRenderPassBeginInfo render_pass_info;
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass;
  render_pass_info.framebuffer = _images[_current_image_index].framebuffer;

  VkRect2D render_area;
  render_area.offset = {0, 0};
  render_area.extent = {_image_width, _image_height};

  render_pass_info.renderArea = render_area;
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport;
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = static_cast<float>(_image_width);
  viewport.height = static_cast<float>(_image_height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor;
  scissor.offset = {0, 0};
  scissor.extent = {_image_width, _image_height};

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void Viewport::_createImages() {
  _depth_image = new GpuImage(
      gpu, display->getDepthFormat(), _image_width, _image_height,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

  for (uint32_t i = 0; i < _images.size(); i++) {
    VkImageViewCreateInfo view_info;
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = _images[i].image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = display->getSwapchainFormat();

    VkComponentMapping components;
    components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components = components;

    VkImageSubresourceRange subresourceRange;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange = subresourceRange;

    if (vkCreateImageView(gpu->device, &view_info, nullptr,
                          &_images[i].image_view) != VK_SUCCESS) {
      log_ftl("Failed to create swapchain image view.");
    }

    std::array<VkImageView, 2> framebuffer_attachments = {_images[i].image_view,
                                                          _depth_image->view};

    VkFramebufferCreateInfo framebuffer_info;
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = renderer->getCompositePass();
    framebuffer_info.attachmentCount = framebuffer_attachments.size();
    framebuffer_info.pAttachments = framebuffer_attachments.data();
    framebuffer_info.width = _image_width;
    framebuffer_info.height = _image_height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(gpu->device, &framebuffer_info, nullptr,
                            &_images[i].framebuffer) != VK_SUCCESS) {
      log_ftl("Failed to create framebuffer.");
    }
  }
}

void Viewport::_destroyImages() {
  for (ViewportImage& image : _images) {
    vkDestroyImageView(gpu->device, image.image_view, nullptr);
    vkDestroyFramebuffer(gpu->device, image.framebuffer, nullptr);
  }
  _images.resize(0);

  if (_depth_image != nullptr) delete _depth_image;
  _depth_image = nullptr;
}

}  // namespace mondradiko
