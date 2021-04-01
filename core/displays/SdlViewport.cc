// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/displays/SdlViewport.h"

#include <cmath>

#include "core/avatars/SpectatorAvatar.h"
#include "core/displays/SdlDisplay.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/renderer/Renderer.h"
#include "log/log.h"
#include "types/containers/vector.h"

namespace mondradiko {
namespace core {

SdlViewport::SdlViewport(GpuInstance* gpu, SdlDisplay* display,
                         Renderer* renderer, SpectatorAvatar* avatar)
    : Viewport(display, gpu, renderer),
      gpu(gpu),
      display(display),
      renderer(renderer),
      avatar(avatar) {
  log_zone;

  int window_width;
  int window_height;
  SDL_Vulkan_GetDrawableSize(display->window, &window_width, &window_height);

  // TODO(marceline-cramer) Better queue sharing
  VkSwapchainCreateInfoKHR swapchain_info{};
  swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_info.surface = display->surface;
  swapchain_info.minImageCount = display->surface_capabilities.minImageCount;
  swapchain_info.imageFormat = display->swapchain_format;
  swapchain_info.imageColorSpace = display->swapchain_color_space;

  VkExtent2D imageExtent{};
  imageExtent.width = static_cast<uint32_t>(window_width);
  imageExtent.height = static_cast<uint32_t>(window_height);
  swapchain_info.imageExtent = imageExtent;

  swapchain_info.imageArrayLayers = 1;
  swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_info.preTransform = display->surface_capabilities.currentTransform;
  swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_info.presentMode = display->swapchain_present_mode;
  swapchain_info.clipped = VK_TRUE;
  swapchain_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(gpu->device, &swapchain_info, nullptr, &swapchain) !=
      VK_SUCCESS) {
    log_ftl("Failed to create swapchain.");
  }

  uint32_t image_count;
  vkGetSwapchainImagesKHR(gpu->device, swapchain, &image_count, nullptr);
  types::vector<VkImage> swapchain_images(image_count);
  vkGetSwapchainImagesKHR(gpu->device, swapchain, &image_count,
                          swapchain_images.data());

  _images.resize(image_count);

  for (uint32_t i = 0; i < image_count; i++) {
    _images[i].image = swapchain_images[i];
  }

  _image_width = window_width;
  _image_height = window_height;
  _createImages();

  on_image_acquire.resize(image_count);

  for (uint32_t i = 0; i < on_image_acquire.size(); i++) {
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(gpu->device, &semaphore_info, nullptr,
                          &on_image_acquire[i]) != VK_SUCCESS) {
      log_ftl("Failed to create image acquisition semaphore.");
    }
  }
}

SdlViewport::~SdlViewport() {
  log_zone;

  vkDeviceWaitIdle(gpu->device);

  for (auto semaphore : on_image_acquire) {
    vkDestroySemaphore(gpu->device, semaphore, nullptr);
  }

  _destroyImages();

  if (swapchain != VK_NULL_HANDLE) {
    vkDestroySwapchainKHR(gpu->device, swapchain, nullptr);
  }
}

void SdlViewport::writeUniform(ViewportUniform* uniform) {
  log_zone;

  uniform->view = avatar->getViewMatrix();

  uniform->projection = glm::perspective(
      glm::radians(80.0f),
      static_cast<float>(_image_width) / static_cast<float>(_image_height),
      0.01f, 1000.0f);

  // Fix GLM matrix to work with Vulkan
  uniform->projection[1][1] *= -1.0;

  uniform->position = avatar->getPosition();
}

VkSemaphore SdlViewport::_acquireImage(uint32_t* image_index) {
  log_zone;

  acquire_image_index++;
  if (acquire_image_index >= on_image_acquire.size()) {
    acquire_image_index = 0;
  }

  vkAcquireNextImageKHR(gpu->device, swapchain, UINT64_MAX,
                        on_image_acquire[acquire_image_index], VK_NULL_HANDLE,
                        image_index);

  return on_image_acquire[acquire_image_index];
}

void SdlViewport::_releaseImage(uint32_t current_image_index,
                                VkSemaphore on_render_finished) {
  log_zone;

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = &on_render_finished;
  present_info.swapchainCount = 1;
  present_info.pSwapchains = &swapchain;
  present_info.pImageIndices = &current_image_index;

  // TODO(marceline-cramer) Better queue management
  vkQueuePresentKHR(gpu->graphics_queue, &present_info);
}

}  // namespace core
}  // namespace mondradiko
