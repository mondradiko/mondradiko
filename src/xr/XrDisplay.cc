/**
 * @file XrDisplay.cc
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

#include "xr/XrDisplay.h"

#include <vulkan/vulkan.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "./build_config.h"
#include "graphics/Viewport.h"
#include "graphics/VulkanInstance.h"
#include "log/log.h"

namespace mondradiko {

#define XR_LOAD_FN_PTR(name, fnPtr)     \
  xrGetInstanceProcAddr(instance, name, \
                        reinterpret_cast<PFN_xrVoidFunction *>(&fnPtr))

static XRAPI_ATTR XrBool32 XRAPI_CALL
debugCallback(XrDebugUtilsMessageSeverityFlagsEXT message_severity,
              XrDebugUtilsMessageTypeFlagsEXT message_type,
              const XrDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  LogLevel severity;

  switch (message_severity) {
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity = LOG_LEVEL_INFO;
      break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity = LOG_LEVEL_WARNING;
      break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
      severity = LOG_LEVEL_ERROR;
      break;
  }

  log("OpenXRValidation", 0, severity, pCallbackData->message);
  return VK_FALSE;
}

XrDisplay::XrDisplay() {
  createInstance();

  if (enable_validation_layers) {
    setupDebugMessenger();
  }

  findSystem();
}

XrDisplay::~XrDisplay() {
  log_dbg("Cleaning up XrDisplay.");

  if (enable_validation_layers && debug_messenger != VK_NULL_HANDLE)
    ext_xrDestroyDebugUtilsMessengerEXT(debug_messenger);

  if (instance != XR_NULL_HANDLE) xrDestroyInstance(instance);
}

bool XrDisplay::getRequirements(VulkanRequirements *requirements) {
  XrGraphicsRequirementsVulkanKHR vulkanRequirements{
      .type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};

  if (ext_xrGetVulkanGraphicsRequirementsKHR(
          instance, system_id, &vulkanRequirements) != XR_SUCCESS) {
    log_err("Failed to get OpenXR Vulkan requirements.");
    return false;
  }

  requirements->min_api_version = vulkanRequirements.minApiVersionSupported;
  requirements->max_api_version = vulkanRequirements.maxApiVersionSupported;

  uint32_t instance_extensions_length;
  ext_xrGetVulkanInstanceExtensionsKHR(instance, system_id, 0,
                                       &instance_extensions_length, nullptr);
  std::vector<char> instance_extensions_list(instance_extensions_length);
  ext_xrGetVulkanInstanceExtensionsKHR(
      instance, system_id, instance_extensions_length,
      &instance_extensions_length, instance_extensions_list.data());
  std::string instance_extensions_names = instance_extensions_list.data();

  // The list of required instance extensions is separated by spaces, so parse
  // it out
  log_inf("Vulkan instance extensions required by OpenXR:");
  requirements->instance_extensions.clear();
  uint32_t start_index = 0;
  for (uint32_t end_index = 0; end_index < instance_extensions_names.size();
       end_index++) {
    if (instance_extensions_names[end_index] == ' ' ||
        instance_extensions_names[end_index] == '\0') {
      std::string extension = instance_extensions_names.substr(
          start_index, end_index - start_index);
      log_inf(extension.c_str());
      requirements->instance_extensions.push_back(extension);
      start_index = end_index + 1;
    }
  }

  // TODO(marceline-cramer) Device/instance extension parsing could be moved
  // into a function
  uint32_t device_extensions_length;
  ext_xrGetVulkanDeviceExtensionsKHR(instance, system_id, 0,
                                     &device_extensions_length, nullptr);
  std::vector<char> device_extensions_list(device_extensions_length);
  ext_xrGetVulkanDeviceExtensionsKHR(
      instance, system_id, device_extensions_length, &device_extensions_length,
      device_extensions_list.data());
  std::string device_extensions_names = device_extensions_list.data();

  // The list of required device extensions is separated by spaces, so parse it
  // out
  log_inf("Vulkan device extensions required by OpenXR:");
  requirements->device_extensions.clear();
  start_index = 0;
  for (uint32_t end_index = 0; end_index < device_extensions_names.size();
       end_index++) {
    if (device_extensions_names[end_index] == ' ' ||
        device_extensions_names[end_index] == '\0') {
      std::string extension =
          device_extensions_names.substr(start_index, end_index - start_index);
      log_inf(extension.c_str());
      requirements->device_extensions.push_back(extension);
      start_index = end_index + 1;
    }
  }

  return true;
}

bool XrDisplay::getVulkanDevice(VkInstance vk_instance,
                                VkPhysicalDevice *vk_physical_device) {
  if (ext_xrGetVulkanGraphicsDeviceKHR(instance, system_id, vk_instance,
                                       vk_physical_device) != XR_SUCCESS) {
    log_err("Failed to get Vulkan physical device.");
    return false;
  }

  return true;
}

void XrDisplay::populateDebugMessengerCreateInfo(
    XrDebugUtilsMessengerCreateInfoEXT *createInfo) {
  *createInfo = {
      .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                           XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                      XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                      XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                      XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT,
      .userCallback = debugCallback};
}

void XrDisplay::createInstance() {
  log_dbg("Creating OpenXR instance.");

  XrApplicationInfo appInfo{.applicationVersion = XR_MAKE_VERSION(0, 0, 0),
                            .engineVersion = MONDRADIKO_OPENXR_VERSION,
                            .apiVersion = XR_MAKE_VERSION(1, 0, 0)};

  snprintf(appInfo.applicationName, XR_MAX_APPLICATION_NAME_SIZE,
           "Mondradiko Client");
  snprintf(appInfo.engineName, XR_MAX_ENGINE_NAME_SIZE, MONDRADIKO_NAME);

  std::vector<const char *> extensions{XR_KHR_VULKAN_ENABLE_EXTENSION_NAME,
                                       XR_EXT_DEBUG_UTILS_EXTENSION_NAME};

  // TODO(marceline-cramer) Validation layers
  XrInstanceCreateInfo instance_info{
      .type = XR_TYPE_INSTANCE_CREATE_INFO,
      .applicationInfo = appInfo,
      .enabledApiLayerCount = 0,
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .enabledExtensionNames = extensions.data()};

  XrResult result = xrCreateInstance(&instance_info, &instance);

  if (result != XR_SUCCESS || instance == nullptr) {
    log_ftl("Failed to create OpenXR instance. Is an OpenXR runtime running?");
  }

  XR_LOAD_FN_PTR("xrCreateDebugUtilsMessengerEXT",
                 ext_xrCreateDebugUtilsMessengerEXT);

  XR_LOAD_FN_PTR("xrDestroyDebugUtilsMessengerEXT",
                 ext_xrDestroyDebugUtilsMessengerEXT);

  XR_LOAD_FN_PTR("xrGetVulkanGraphicsRequirementsKHR",
                 ext_xrGetVulkanGraphicsRequirementsKHR);

  XR_LOAD_FN_PTR("xrGetVulkanInstanceExtensionsKHR",
                 ext_xrGetVulkanInstanceExtensionsKHR);

  XR_LOAD_FN_PTR("xrGetVulkanGraphicsDeviceKHR",
                 ext_xrGetVulkanGraphicsDeviceKHR);

  XR_LOAD_FN_PTR("xrGetVulkanDeviceExtensionsKHR",
                 ext_xrGetVulkanDeviceExtensionsKHR);
}

void XrDisplay::setupDebugMessenger() {
  XrDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(&createInfo);

  if (ext_xrCreateDebugUtilsMessengerEXT(instance, &createInfo,
                                         &debug_messenger) != XR_SUCCESS) {
    log_ftl("Failed to create OpenXR debug messenger.");
  }
}

void XrDisplay::findSystem() {
  log_dbg("Choosing OpenXR system.");

  XrSystemGetInfo systemInfo{.type = XR_TYPE_SYSTEM_GET_INFO,
                             .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY};

  if (xrGetSystem(instance, &systemInfo, &system_id) != XR_SUCCESS) {
    log_ftl("Failed to find OpenXR HMD.");
  }
}

}  // namespace mondradiko
