/**
 * @file OpenXrDisplay.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements an OpenXR VR display interface.
 * @date 2020-11-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

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
  VkImageLayout getFinalLayout() override {
    // OpenXR expects the final swapchain image layout to be
    // _COLOR_ATTACHMENT_OPTIMAL
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

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

  void createViewports(Renderer*);

  XrDebugUtilsMessengerCreateInfoEXT debug_messenger_info;

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
