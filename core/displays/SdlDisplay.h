/**
 * @file SdlDisplay.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements an SDL window display and input.
 * @date 2020-11-09
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/common/api_headers.h"
#include "core/displays/DisplayInterface.h"

namespace mondradiko {

// Forward declarations
class SdlViewport;
class GpuInstance;

class SdlDisplay : public DisplayInterface {
 public:
  SdlDisplay();
  ~SdlDisplay();

  bool getVulkanRequirements(VulkanRequirements*) final;
  bool getVulkanDevice(VkInstance, VkPhysicalDevice*) final;
  bool createSession(GpuInstance*) final;
  void destroySession() final;

  VkFormat getSwapchainFormat() final { return swapchain_format; }
  VkImageLayout getFinalLayout() final {
    // SDL expects the final swapchain image layout to be
    // _PRESENT_SRC_KHR
    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  }

  void pollEvents(DisplayPollEventsInfo*) final;
  void beginFrame(DisplayBeginFrameInfo*) final;
  void acquireViewports(std::vector<ViewportInterface*>*) final;
  void endFrame(DisplayBeginFrameInfo*) final;

  SDL_Window* window = nullptr;
  SDL_Surface* window_surface;

  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkSurfaceCapabilitiesKHR surface_capabilities;

  VkFormat swapchain_format;
  VkColorSpaceKHR swapchain_color_space;
  VkPresentModeKHR swapchain_present_mode;

  SdlViewport* main_viewport = nullptr;

 private:
  GpuInstance* gpu;

  // TODO(marceline-cramer) Move this into GpuInstance
  uint32_t present_queue_family;
  VkQueue present_queue = VK_NULL_HANDLE;
};

}  // namespace mondradiko
