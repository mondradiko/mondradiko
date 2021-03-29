// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/gpu/GpuInstance.h"

#include <cstring>
#include <set>

#include "core/displays/DisplayInterface.h"
#include "core/gpu/vulkan_validation.h"
#include "log/log.h"
#include "types/build_config.h"

namespace mondradiko {
namespace core {

GpuInstance::GpuInstance(DisplayInterface* display) : display(display) {
  log_zone;

  VulkanRequirements requirements;
  display->getVulkanRequirements(&requirements);

  if (enableValidationLayers) {
    if (!checkValidationLayerSupport()) {
      log_wrn("Vulkan validation layers requested, but not available.");
      enableValidationLayers = false;
    }
  }

  createInstance(&requirements);

  if (enableValidationLayers) {
    setupDebugMessenger();
  }

  findPhysicalDevice(display);
  findQueueFamilies();
  createLogicalDevice(&requirements);
  createCommandPool();
  createAllocator();
}

GpuInstance::~GpuInstance() {
  log_zone;

  display->destroySession();

  vkDeviceWaitIdle(device);

  if (allocator != nullptr) vmaDestroyAllocator(allocator);

  if (command_pool != VK_NULL_HANDLE)
    vkDestroyCommandPool(device, command_pool, nullptr);

  if (device != VK_NULL_HANDLE) vkDestroyDevice(device, nullptr);

  if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE)
    ext_vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

  if (instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
}

bool GpuInstance::findFormatFromOptions(
    const types::vector<VkFormat>* options,
    const types::vector<VkFormat>* candidates, VkFormat* selected) {
  for (auto candidate : *candidates) {
    for (auto option : *options) {
      if (candidate == option) {
        *selected = option;
        return true;
      }
    }
  }

  return false;
}

bool GpuInstance::findSupportedFormat(const types::vector<VkFormat>* options,
                                      VkImageTiling tiling,
                                      VkFormatFeatureFlags features,
                                      VkFormat* selected) {
  for (VkFormat format : *options) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      *selected = format;
      return true;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features) {
      *selected = format;
      return true;
    }
  }

  return false;
}

VkCommandBuffer GpuInstance::beginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = command_pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void GpuInstance::endSingleTimeCommands(VkCommandBuffer command_buffer) {
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &command_buffer;

  vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue);

  vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

bool GpuInstance::checkValidationLayerSupport() {
  log_zone;

  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  types::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* layer_name : validationLayers) {
    bool layer_found = false;

    for (const auto& layer_properties : available_layers) {
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        log_msg_fmt("layer_found %s", layer_name);
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      log_msg_fmt("!layer_found %s", layer_name);
      return false;
    }
  }

  return true;
}

void GpuInstance::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
  *createInfo = {};
  createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo->messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo->pfnUserCallback = debugCallbackVulkan;
}

void GpuInstance::createInstance(VulkanRequirements* requirements) {
  log_zone;

  types::vector<const char*> extensionNames;
  for (uint32_t i = 0; i < requirements->instance_extensions.size(); i++) {
    extensionNames.push_back(requirements->instance_extensions[i].c_str());
  }

  extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Mondradiko";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
  appInfo.pEngineName = MONDRADIKO_NAME;
  appInfo.engineVersion = MONDRADIKO_VULKAN_VERSION;
  appInfo.apiVersion = requirements->min_api_version;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(extensionNames.size());
  createInfo.ppEnabledExtensionNames = extensionNames.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(&debugCreateInfo);
    createInfo.pNext =
        reinterpret_cast<VkInstanceCreateInfo*>(&debugCreateInfo);
  }

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    log_ftl("Failed to create Vulkan instance.");
  }

  ext_vkCreateDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
  ext_vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkDestroyDebugUtilsMessengerEXT");
}

void GpuInstance::setupDebugMessenger() {
  log_zone;

  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(&createInfo);

  if (ext_vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                         &debugMessenger) != VK_SUCCESS) {
    log_ftl("Failed to create Vulkan debug messenger.");
  }
}

void GpuInstance::findPhysicalDevice(DisplayInterface* display) {
  log_zone;

  if (!display->getVulkanDevice(instance, &physical_device)) {
    log_ftl("Failed to find Vulkan physical device.");
  }

  vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
}

void GpuInstance::findQueueFamilies() {
  log_zone;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
                                           nullptr);
  types::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
                                           queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      graphics_queue_family = i;
      break;
    }
  }
}

void GpuInstance::createLogicalDevice(VulkanRequirements* requirements) {
  log_zone;

  types::vector<const char*> extensionNames;
  for (uint32_t i = 0; i < requirements->device_extensions.size(); i++) {
    extensionNames.push_back(requirements->device_extensions[i].c_str());
  }

  types::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {graphics_queue_family};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.multiViewport = VK_TRUE;
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(extensionNames.size());
  createInfo.ppEnabledExtensionNames = extensionNames.data();
  createInfo.pEnabledFeatures = &deviceFeatures;

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    log_ftl("Failed to create Vulkan logical device.");
  }

  vkGetDeviceQueue(device, graphics_queue_family, 0, &graphics_queue);
}

void GpuInstance::createCommandPool() {
  log_zone;

  VkCommandPoolCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                     VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  createInfo.queueFamilyIndex = graphics_queue_family;

  if (vkCreateCommandPool(device, &createInfo, nullptr, &command_pool) !=
      VK_SUCCESS) {
    log_ftl("Failed to create Vulkan command pool.");
  }
}

void GpuInstance::createAllocator() {
  log_zone;

  VmaAllocatorCreateInfo createInfo{};
  createInfo.physicalDevice = physical_device;
  createInfo.device = device;
  createInfo.instance = instance;

  if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
    log_ftl("Failed to create Vulkan memory allocator.");
  }
}

}  // namespace core
}  // namespace mondradiko
