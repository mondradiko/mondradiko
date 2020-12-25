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

#include "core/common/openxr_headers.h"
#include "core/displays/DisplayInterface.h"

namespace mondradiko {

// Forward declarations
class OpenXrViewport;
class Renderer;

class OpenXrDisplay : public DisplayInterface {
 public:
  OpenXrDisplay();
  ~OpenXrDisplay();

  bool getVulkanRequirements(VulkanRequirements*) final;
  bool getVulkanDevice(VkInstance, VkPhysicalDevice*) final;
  bool createSession(GpuInstance*) final;
  void destroySession() final;

  VkFormat getSwapchainFormat() final { return swapchain_format; };
  VkImageLayout getFinalLayout() final {
    // OpenXR expects the final swapchain image layout to be
    // _COLOR_ATTACHMENT_OPTIMAL
    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }
  VkFormat getDepthFormat() final {
    // TODO(marceline-cramer) OxrDisplay depth image
    return VK_FORMAT_D32_SFLOAT;
  }

  void pollEvents(DisplayPollEventsInfo*) final;
  void beginFrame(DisplayBeginFrameInfo*) final;
  void acquireViewports(std::vector<ViewportInterface*>*) final;
  void endFrame(DisplayBeginFrameInfo*) final;

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
