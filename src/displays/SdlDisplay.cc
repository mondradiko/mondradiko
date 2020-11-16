/**
 * @file SdlDisplay.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements an SDL window display and input.
 * @date 2020-11-09
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

#include "displays/SdlDisplay.h"

#include "./config.h"
#include "log/log.h"

namespace mondradiko {

SdlDisplay::SdlDisplay() {
  log_dbg("Creating SDL display.");

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    log_ftl("Failed to initialize SDL.");
  }

  window = SDL_CreateWindow(MONDRADIKO_NAME, SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, 800, 600,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

  if (!window) {
    log_ftl("Failed to create SDL window.");
  }
}

SdlDisplay::~SdlDisplay() {
  if (window != nullptr) SDL_DestroyWindow(window);
  SDL_Quit();
}

bool SdlDisplay::getVulkanRequirements(VulkanRequirements* requirements) {
  requirements->min_api_version = VK_MAKE_VERSION(1, 0, 0);
  requirements->max_api_version = VK_MAKE_VERSION(1, 2, 0);
  requirements->instance_extensions.resize(0);
  requirements->device_extensions.resize(0);

  uint32_t instance_extension_count;
  SDL_Vulkan_GetInstanceExtensions(window, &instance_extension_count, nullptr);
  std::vector<const char*> instance_extension_names(instance_extension_count);
  SDL_Vulkan_GetInstanceExtensions(window, &instance_extension_count,
                                   instance_extension_names.data());

  for (auto& instance_extension : instance_extension_names) {
    requirements->instance_extensions.push_back(instance_extension);
  }

  requirements->device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  return true;
}

bool SdlDisplay::getVulkanDevice(VkInstance instance,
                                 VkPhysicalDevice* physical_device) {
  window_surface = SDL_GetWindowSurface(window);

  // Clear window
  SDL_FillRect(window_surface, nullptr, 0);
  SDL_UpdateWindowSurface(window);

  if (SDL_Vulkan_CreateSurface(window, instance, &surface) != SDL_TRUE) {
    log_err("Failed to create SDL window surface.");
    return false;
  }

  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

  // TODO(marceline-cramer) Move physical device selection/queue creation to
  // VulkanInstance
  // TODO(marceline-cramer) Pick best device
  if (device_count > 0) {
    *physical_device = devices[0];

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device,
                                             &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        *physical_device, &queue_family_count, queue_families.data());

    bool any_present_support = false;
    for (uint32_t i = 0; i < queue_family_count; i++) {
      VkBool32 present_support = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device, i, surface,
                                           &present_support);

      if (present_support) any_present_support = true;
    }

    if (!any_present_support) {
      log_err("No Vulkan physical device supports a present queue.");
      return false;
    }
  } else {
    log_err("Could not find suitable Vulkan physical device.");
    return false;
  }

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*physical_device, surface,
                                            &surface_capabilities);

  uint32_t format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device, surface, &format_count,
                                       nullptr);
  std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(*physical_device, surface, &format_count,
                                       surface_formats.data());

  swapchain_format = VK_FORMAT_MAX_ENUM;
  for (const auto& surface_format : surface_formats) {
    if (surface_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
        surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      swapchain_format = surface_format.format;
      swapchain_color_space = surface_format.colorSpace;
    }
  }

  if (swapchain_format == VK_FORMAT_MAX_ENUM) {
    log_err("Failed to find suitable surface format.");
    return false;
  }

  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(*physical_device, surface,
                                            &present_mode_count, nullptr);
  std::vector<VkPresentModeKHR> present_modes(present_mode_count);
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      *physical_device, surface, &present_mode_count, present_modes.data());

  swapchain_present_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
  for (const auto& surface_present_mode : present_modes) {
    if (surface_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      swapchain_present_mode = surface_present_mode;
    }
  }

  if (swapchain_present_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
    log_err("Failed to find suitable swapchain present mode.");
    return false;
  }

  return true;
}

bool SdlDisplay::createSession(GpuInstance* _gpu) {
  gpu = _gpu;

  // TODO(marceline-cramer) Better queue sharing
  VkSwapchainCreateInfoKHR swapchain_info{
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .surface = surface,
      .minImageCount = surface_capabilities.minImageCount,
      .imageFormat = swapchain_format,
      .imageColorSpace = swapchain_color_space,
      .imageExtent = surface_capabilities.currentExtent,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .preTransform = surface_capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = swapchain_present_mode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE};

  if (vkCreateSwapchainKHR(gpu->device, &swapchain_info, nullptr, &swapchain) !=
      VK_SUCCESS) {
    log_err("Failed to create swapchain.");
    return false;
  }

  return true;
}

void SdlDisplay::destroySession() {
  if (main_viewport != nullptr) delete main_viewport;
  if (swapchain != VK_NULL_HANDLE)
    vkDestroySwapchainKHR(gpu->device, swapchain, nullptr);
  if (surface != VK_NULL_HANDLE)
    vkDestroySurfaceKHR(gpu->instance, surface, nullptr);

  main_viewport = nullptr;
  swapchain = VK_NULL_HANDLE;
  surface = VK_NULL_HANDLE;
}

void SdlDisplay::pollEvents(DisplayPollEventsInfo* poll_info) {
  if (main_viewport == nullptr) {
    main_viewport = new SdlViewport(gpu, this, poll_info->renderer);
  }

  poll_info->should_quit = false;
  poll_info->should_run = true;
}

void SdlDisplay::beginFrame(DisplayBeginFrameInfo* frame_info) {
  // TODO(marceline-cramer) SDL delta time
  if (main_viewport != nullptr) {
    frame_info->should_render = true;
  } else {
    frame_info->should_render = false;
  }
}

void SdlDisplay::acquireViewports(std::vector<ViewportInterface*>* viewports) {
  viewports->resize(1);
  viewports->at(0) = main_viewport;
}

void SdlDisplay::endFrame(DisplayBeginFrameInfo* frame_info) {}

}  // namespace mondradiko
