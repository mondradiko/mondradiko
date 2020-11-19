/**
 * @file SdlViewport.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements access to an SDL window swapchain.
 * @date 2020-11-10
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

#include "displays/SdlViewport.h"

#include "displays/SdlDisplay.h"
#include "log/log.h"

namespace mondradiko {

SdlViewport::SdlViewport(GpuInstance* gpu, SdlDisplay* display,
                         Renderer* renderer)
    : gpu(gpu), display(display), renderer(renderer) {
  log_dbg("Creating SDL viewport.");

  image_width = display->surface_capabilities.currentExtent.width;
  image_height = display->surface_capabilities.currentExtent.height;

  uint32_t image_count;
  vkGetSwapchainImagesKHR(gpu->device, display->swapchain, &image_count,
                          nullptr);
  std::vector<VkImage> swapchain_images(image_count);
  vkGetSwapchainImagesKHR(gpu->device, display->swapchain, &image_count,
                          swapchain_images.data());

  images.resize(image_count);

  for (uint32_t i = 0; i < image_count; i++) {
    images[i].image = swapchain_images[i];

    VkImageViewCreateInfo view_info{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = images[i].image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = display->getSwapchainFormat(),
        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                       .a = VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1}};

    if (vkCreateImageView(gpu->device, &view_info, nullptr,
                          &images[i].image_view) != VK_SUCCESS) {
      log_ftl("Failed to create swapchain image view.");
    }

    VkFramebufferCreateInfo framebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderer->composite_pass,
        .attachmentCount = 1,
        .pAttachments = &images[i].image_view,
        .width = image_width,
        .height = image_height,
        .layers = 1};

    if (vkCreateFramebuffer(gpu->device, &framebufferCreateInfo, nullptr,
                            &images[i].framebuffer) != VK_SUCCESS) {
      log_ftl("Failed to create framebuffer.");
    }
  }

  VkFenceCreateInfo fence_info{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};

  if (vkCreateFence(gpu->device, &fence_info, nullptr, &on_image_available) !=
      VK_SUCCESS) {
    log_ftl("Failed to create image availability fence.");
  }
}

SdlViewport::~SdlViewport() {
  log_dbg("Destroying SDL viewport.");

  if (on_image_available != VK_NULL_HANDLE) {
    vkDestroyFence(gpu->device, on_image_available, nullptr);
  }

  for (ViewportImage& image : images) {
    vkDestroyImageView(gpu->device, image.image_view, nullptr);
    vkDestroyFramebuffer(gpu->device, image.framebuffer, nullptr);
  }
}

void SdlViewport::acquire() {
  vkAcquireNextImageKHR(gpu->device, display->swapchain, UINT64_MAX,
                        VK_NULL_HANDLE, on_image_available,
                        &current_image_index);

  // Block CPU until SDL swapchain image is available
  // This is suboptimal, but OpenXR blocks CPU too, so...
  vkWaitForFences(gpu->device, 1, &on_image_available, VK_TRUE, UINT64_MAX);
  vkResetFences(gpu->device, 1, &on_image_available);
}

void SdlViewport::beginRenderPass(VkCommandBuffer command_buffer,
                                  VkRenderPass render_pass) {
  std::array<VkClearValue, 1> clear_values;

  clear_values[0].color = {0.2, 0.0, 0.0, 1.0};

  VkRenderPassBeginInfo renderPassInfo{
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .renderPass = render_pass,
      .framebuffer = images[current_image_index].framebuffer,
      .renderArea = {.offset = {0, 0}, .extent = {image_width, image_height}},
      .clearValueCount = static_cast<uint32_t>(clear_values.size()),
      .pClearValues = clear_values.data()};

  vkCmdBeginRenderPass(command_buffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport{.x = 0,
                      .y = 0,
                      .width = static_cast<float>(image_width),
                      .height = static_cast<float>(image_height),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};

  VkRect2D scissor{.offset = {0, 0}, .extent = {image_width, image_height}};

  vkCmdSetViewport(command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void SdlViewport::writeUniform(ViewportUniform* uniform) {
  // TODO(marceline-cramer) Add WASD+Mouse control
  uniform->view =
      glm::lookAt(glm::vec3(2.0, 2.0, 2.0), glm::vec3(0.0, 0.0, 0.0),
                  glm::vec3(0.0, 1.0, 0.0));
  uniform->projection = glm::perspective(
      glm::radians(90.0f),
      static_cast<float>(image_width) / static_cast<float>(image_height), 0.01f,
      1000.0f);

  // Fix GLM matrix to work with Vulkan
  uniform->projection[1][1] *= -1.0;
}

void SdlViewport::release() {
  // TODO(marceline-cramer) Add render finished semaphores
  VkPresentInfoKHR present_info{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                .swapchainCount = 1,
                                .pSwapchains = &display->swapchain,
                                .pImageIndices = &current_image_index};

  // TODO(marceline-cramer) Better queue management
  vkQueuePresentKHR(gpu->graphics_queue, &present_info);
}

}  // namespace mondradiko
