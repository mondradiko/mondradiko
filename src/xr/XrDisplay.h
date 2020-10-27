/**
 * @file XrDisplay.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and manages low-level OpenXR objects like instance, device,
 * debug messenger, etc.
 * @date 2020-10-24
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

#ifndef SRC_XR_XRDISPLAY_H_
#define SRC_XR_XRDISPLAY_H_

#include <string>
#include <vector>

#include "src/api_headers.h"

// Forward declarations because of codependency
class VulkanInstance;
class Viewport;

struct VulkanRequirements {
  uint32_t minApiVersion;
  uint32_t maxApiVersion;

  std::vector<std::string> instanceExtensions;

  VkPhysicalDevice physicalDevice;

  std::vector<std::string> deviceExtensions;
};

class XrDisplay {
 public:
  XrDisplay();
  ~XrDisplay();

  bool getRequirements(VulkanRequirements*);
  bool getVulkanDevice(VkInstance, VkPhysicalDevice*);

  bool enableValidationLayers = true;

  XrInstance instance = XR_NULL_HANDLE;
  XrDebugUtilsMessengerEXT debugMessenger = XR_NULL_HANDLE;
  XrSystemId systemId = XR_NULL_SYSTEM_ID;

 private:
  void populateDebugMessengerCreateInfo(XrDebugUtilsMessengerCreateInfoEXT*);
  void createInstance();
  void setupDebugMessenger();
  void findSystem();

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

#endif  // SRC_XR_XRDISPLAY_H_
