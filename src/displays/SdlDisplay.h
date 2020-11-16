/**
 * @file SdlDisplay.h
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

#ifndef SRC_DISPLAYS_SDLDISPLAY_H_
#define SRC_DISPLAYS_SDLDISPLAY_H_

#include "displays/DisplayInterface.h"
#include "displays/SdlViewport.h"
#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class SdlDisplay : public DisplayInterface {
 public:
  SdlDisplay();
  ~SdlDisplay();

  bool getRequirements(VulkanRequirements*) override;
  bool getVulkanDevice(VkInstance, VkPhysicalDevice*) override;
  bool createSession(VulkanInstance*) override;
  void destroySession() override;

  VkFormat getSwapchainFormat() override { return swapchain_format; }

  void pollEvents(DisplayPollEventsInfo*) override;
  void beginFrame(DisplayBeginFrameInfo*) override;
  void acquireViewports(std::vector<ViewportInterface*>*) override;
  void endFrame(DisplayBeginFrameInfo*) override;

  SDL_Window* window = nullptr;
  SDL_Surface* window_surface;

  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkSurfaceCapabilitiesKHR surface_capabilities;

  VkSwapchainKHR swapchain = VK_NULL_HANDLE;
  VkFormat swapchain_format;
  VkColorSpaceKHR swapchain_color_space;
  VkPresentModeKHR swapchain_present_mode;

  SdlViewport* main_viewport = nullptr;

 private:
  VulkanInstance* vulkan_instance;

  // TODO(marceline-cramer) Move this into VulkanInstance
  uint32_t present_queue_family;
  VkQueue present_queue = VK_NULL_HANDLE;
};

}  // namespace mondradiko

#endif  // SRC_DISPLAYS_SDLDISPLAY_H_
