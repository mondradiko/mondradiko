/**
 * @file OpenXrDisplay.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements an OpenXR VR display interface.
 * @date 2020-11-08
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

#ifndef SRC_DISPLAYS_OPENXRDISPLAY_H_
#define SRC_DISPLAYS_OPENXRDISPLAY_H_

#include <string>
#include <vector>

#include "displays/DisplayInterface.h"
#include "displays/OpenXrViewport.h"
#include "renderer/Renderer.h"

namespace mondradiko {

class OpenXrDisplay : public DisplayInterface {
 public:
  OpenXrDisplay();
  ~OpenXrDisplay();

  bool getVulkanRequirements(VulkanRequirements*) override;
  bool getVulkanDevice(VkInstance, VkPhysicalDevice*) override;
  bool createSession(GpuInstance*) override;
  void destroySession() override;

  VkFormat getSwapchainFormat() override { return swapchain_format; };

  void pollEvents(DisplayPollEventsInfo*) override;
  void beginFrame(DisplayBeginFrameInfo*) override;
  void acquireViewports(std::vector<ViewportInterface*>*) override;
  void endFrame(DisplayBeginFrameInfo*) override;

  bool enable_validation_layers = true;

  XrInstance instance = XR_NULL_HANDLE;
  XrDebugUtilsMessengerEXT debug_messenger = XR_NULL_HANDLE;
  XrSystemId system_id = XR_NULL_SYSTEM_ID;

  XrSession session = XR_NULL_HANDLE;
  XrSessionState session_state = XR_SESSION_STATE_UNKNOWN;
  XrFrameState current_frame_state;

  XrSpace stage_space = XR_NULL_HANDLE;

 private:
  GpuInstance* gpu;

  void populateDebugMessengerCreateInfo(XrDebugUtilsMessengerCreateInfoEXT*);
  void createInstance();
  void setupDebugMessenger();
  void findSystem();

  void createViewports(Renderer*);

  VkFormat swapchain_format;

  std::vector<OpenXrViewport*> viewports;

  PFN_xrCreateDebugUtilsMessengerEXT ext_xrCreateDebugUtilsMessengerEXT =
      nullptr;
  PFN_xrDestroyDebugUtilsMessengerEXT ext_xrDestroyDebugUtilsMessengerEXT =
      nullptr;
  PFN_xrGetVulkanGraphicsRequirementsKHR
      ext_xrGetVulkanGraphicsRequirementsKHR = nullptr;
  PFN_xrGetVulkanInstanceExtensionsKHR ext_xrGetVulkanInstanceExtensionsKHR =
      nullptr;
  PFN_xrGetVulkanGraphicsDeviceKHR ext_xrGetVulkanGraphicsDeviceKHR = nullptr;
  PFN_xrGetVulkanDeviceExtensionsKHR ext_xrGetVulkanDeviceExtensionsKHR =
      nullptr;
};

}  // namespace mondradiko

#endif  // SRC_DISPLAYS_OPENXRDISPLAY_H_
