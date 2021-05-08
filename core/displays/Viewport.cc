// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/displays/Viewport.h"

#include "core/displays/Display.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"

namespace mondradiko {
namespace core {

void Viewport::beginRender(VkCommandBuffer command_buffer,
                           VkRenderPass render_pass) {
  log_zone;

  std::vector<VkClearValue> clear_values;

  if (display->getSampleCount() > 1) {
    clear_values.resize(6);
    clear_values[0].depthStencil = {1.0f};
    clear_values[1] = clear_values[0];
    clear_values[2].color = {0.0, 0.0, 0.0, 1.0};
    clear_values[3] = clear_values[2];
    clear_values[4].color = {0.0, 0.0, 0.0, 0.0};
    clear_values[5] = clear_values[4];
  } else {
    clear_values.resize(3);
    clear_values[0].depthStencil = {1.0f};
    clear_values[1].color = {0.0, 0.0, 0.0, 1.0};
    clear_values[2].color = {0.0, 0.0, 0.0, 0.0};
  }

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass;
  render_pass_info.framebuffer = _framebuffer;

  VkRect2D render_area{};
  render_area.offset = {0, 0};
  render_area.extent = {_image_width, _image_height};

  render_pass_info.renderArea = render_area;
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = static_cast<float>(_image_width);
  viewport.height = static_cast<float>(_image_height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {_image_width, _image_height};

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void Viewport::beginComposite(VkCommandBuffer command_buffer,
                              VkRenderPass render_pass) {
  log_zone;

  std::array<VkClearValue, 4> clear_values;

  clear_values[0].color = {0.2, 0.0, 0.0, 1.0};
  clear_values[1].depthStencil = {1.0f};
  clear_values[2].color = {0.0, 0.0, 0.0, 1.0};
  clear_values[3].color = {0.0, 0.0, 0.0, 0.0};

  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = render_pass;
  render_pass_info.framebuffer = _images[_current_image_index].framebuffer;

  VkRect2D render_area{};
  render_area.offset = {0, 0};
  render_area.extent = {_image_width, _image_height};

  render_pass_info.renderArea = render_area;
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();

  vkCmdBeginRenderPass(command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{};
  viewport.x = 0;
  viewport.y = 0;
  viewport.width = static_cast<float>(_image_width);
  viewport.height = static_cast<float>(_image_height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {_image_width, _image_height};

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void Viewport::_createImages() {
  bool msaa_enabled = display->getSampleCount() > 1;

  {
    log_zone_named("Create offscreen images");

    uint32_t w = _image_width;
    uint32_t h = _image_height;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY;

    _sample_count = display->getSampleCount();

    VkImageUsageFlags msaa_usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
    VkImageUsageFlags input_usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    VkImageUsageFlags color_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageUsageFlags depth_usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkFormat hdr_format = display->getHdrFormat();
    VkFormat overlay_format = VK_FORMAT_R8G8B8A8_SRGB;
    VkFormat depth_format = display->getDepthFormat();

    _hdr_image = std::make_unique<GpuImage>(
        gpu, hdr_format, w, h, color_usage | input_usage, memory_usage);

    _overlay_image = std::make_unique<GpuImage>(
        gpu, overlay_format, w, h, color_usage | input_usage, memory_usage);

    _depth_image = std::make_unique<GpuImage>(gpu, depth_format, w, h,
                                              depth_usage, memory_usage);

    if (msaa_enabled) {
      _hdr_msaa =
          std::make_unique<GpuImage>(gpu, hdr_format, w, h, _sample_count,
                                     color_usage | msaa_usage, memory_usage);
      _overlay_msaa =
          std::make_unique<GpuImage>(gpu, overlay_format, w, h, _sample_count,
                                     color_usage | msaa_usage, memory_usage);
      _depth_msaa =
          std::make_unique<GpuImage>(gpu, depth_format, w, h, _sample_count,
                                     depth_usage | msaa_usage, memory_usage);
    }
  }

  {
    log_zone_named("Create offscreen framebuffer");

    types::vector<VkImageView> framebuffer_attachments;

    if (msaa_enabled) {
      framebuffer_attachments = {
          _depth_image->getView(),   _depth_msaa->getView(),
          _hdr_image->getView(),     _hdr_msaa->getView(),
          _overlay_image->getView(), _overlay_msaa->getView()};
    } else {
      framebuffer_attachments = {_depth_image->getView(), _hdr_image->getView(),
                                 _overlay_image->getView()};
    }

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = renderer->getMainRenderPass();
    framebuffer_info.attachmentCount = framebuffer_attachments.size();
    framebuffer_info.pAttachments = framebuffer_attachments.data();
    framebuffer_info.width = _image_width;
    framebuffer_info.height = _image_height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(gpu->device, &framebuffer_info, nullptr,
                            &_framebuffer) != VK_SUCCESS) {
      log_ftl("Failed to create framebuffer.");
    }
  }

  for (uint32_t i = 0; i < _images.size(); i++) {
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = _images[i].image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = display->getSwapchainFormat();

    VkComponentMapping components{};
    components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components = components;

    VkImageSubresourceRange subresourceRange{};
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

    std::array<VkImageView, 4> framebuffer_attachments = {
        _images[i].image_view, _depth_image->getView(), _hdr_image->getView(),
        _overlay_image->getView()};

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = renderer->getCompositeRenderPass();
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

  if (_framebuffer != VK_NULL_HANDLE)
    vkDestroyFramebuffer(gpu->device, _framebuffer, nullptr);
  _framebuffer = VK_NULL_HANDLE;
}

}  // namespace core
}  // namespace mondradiko
