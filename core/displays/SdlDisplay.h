// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <vector>

#include "core/displays/DisplayInterface.h"
#include "lib/include/sdl2_headers.h"

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
  VkFormat getDepthFormat() final { return depth_format; }

  void pollEvents(DisplayPollEventsInfo*) final;
  void beginFrame(DisplayBeginFrameInfo*) final;
  void acquireViewports(std::vector<Viewport*>*) final;
  void endFrame(DisplayBeginFrameInfo*) final;

  SDL_Window* window = nullptr;

  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkSurfaceCapabilitiesKHR surface_capabilities;

  VkFormat swapchain_format;
  VkColorSpaceKHR swapchain_color_space;
  VkPresentModeKHR swapchain_present_mode;

  VkFormat depth_format;

  SdlViewport* main_viewport = nullptr;

 private:
  GpuInstance* gpu;

  // TODO(marceline-cramer) Move this into GpuInstance
  uint32_t present_queue_family;
  VkQueue present_queue = VK_NULL_HANDLE;

  double mouse_x;
  double mouse_y;

  double last_frame_time = -1.0;

  const Uint8* key_state;
};

}  // namespace mondradiko
