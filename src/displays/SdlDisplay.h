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

#ifndef SRC_DISPLAYS_SDLDISPLAY_H_
#define SRC_DISPLAYS_SDLDISPLAY_H_

#include <vector>

#include "displays/DisplayInterface.h"
#include "displays/SdlViewport.h"
#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class SdlDisplay : public DisplayInterface {
 public:
  SdlDisplay();
  ~SdlDisplay();

  bool getVulkanRequirements(VulkanRequirements*) override;
  bool getVulkanDevice(VkInstance, VkPhysicalDevice*) override;
  bool createSession(GpuInstance*) override;
  void destroySession() override;

  VkFormat getSwapchainFormat() override { return swapchain_format; }
  VkImageLayout getFinalLayout() override {
    // SDL expects the final swapchain image layout to be
    // _PRESENT_SRC_KHR
    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  }

  void pollEvents(DisplayPollEventsInfo*) override;
  void beginFrame(DisplayBeginFrameInfo*) override;
  void acquireViewports(std::vector<ViewportInterface*>*) override;
  void endFrame(DisplayBeginFrameInfo*) override;

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

#endif  // SRC_DISPLAYS_SDLDISPLAY_H_
