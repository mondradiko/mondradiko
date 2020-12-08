/**
 * @file GpuInstance.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Manages all low-level Vulkan objects such as device, debug messenger,
 * VMA allocator, etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/gpu/GpuInstance.h"

#include <cstring>
#include <set>

#include "./build_config.h"
#include "core/log/log.h"

namespace mondradiko {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  LogLevel severity;

  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      severity = LOG_LEVEL_INFO;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      severity = LOG_LEVEL_WARNING;
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
      severity = LOG_LEVEL_ERROR;
      break;
  }

  // Add "../src/" so that it's clipped off in logging
  log("../src/VulkanValidation", 0, severity, pCallbackData->pMessage);
  return VK_FALSE;
}

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

  vkDeviceWaitIdle(device);

  if (allocator != nullptr) vmaDestroyAllocator(allocator);

  if (command_pool != VK_NULL_HANDLE)
    vkDestroyCommandPool(device, command_pool, nullptr);

  if (device != VK_NULL_HANDLE) vkDestroyDevice(device, nullptr);

  if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE)
    ext_vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

  if (instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
}

bool GpuInstance::findFormatFromOptions(const std::vector<VkFormat>* options,
                                        const std::vector<VkFormat>* candidates,
                                        VkFormat* selected) {
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

VkCommandBuffer GpuInstance::beginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1};

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void GpuInstance::endSingleTimeCommands(VkCommandBuffer command_buffer) {
  vkEndCommandBuffer(command_buffer);

  VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                          .commandBufferCount = 1,
                          .pCommandBuffers = &command_buffer};

  vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(graphics_queue);

  vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

bool GpuInstance::checkValidationLayerSupport() {
  log_zone;

  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char* layer_name : validationLayers) {
    bool layer_found = false;

    for (const auto& layer_properties : available_layers) {
      if (strcmp(layer_name, layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      return false;
    }
  }

  return true;
}

void GpuInstance::populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
  *createInfo = VkDebugUtilsMessengerCreateInfoEXT{
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = debugCallback};
}

void GpuInstance::createInstance(VulkanRequirements* requirements) {
  log_zone;

  std::vector<const char*> extensionNames;
  for (uint32_t i = 0; i < requirements->instance_extensions.size(); i++) {
    extensionNames.push_back(requirements->instance_extensions[i].c_str());
  }

  extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  VkApplicationInfo appInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                            .pApplicationName = "Mondradiko",
                            .applicationVersion = VK_MAKE_VERSION(0, 0, 0),
                            .pEngineName = MONDRADIKO_NAME,
                            .engineVersion = MONDRADIKO_VULKAN_VERSION,
                            .apiVersion = requirements->min_api_version};

  VkInstanceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
      .ppEnabledExtensionNames = extensionNames.data()};

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
}

void GpuInstance::findQueueFamilies() {
  log_zone;

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queueFamilyCount,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
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

  std::vector<const char*> extensionNames;
  for (uint32_t i = 0; i < requirements->device_extensions.size(); i++) {
    extensionNames.push_back(requirements->device_extensions[i].c_str());
  }

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {graphics_queue_family};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    queueCreateInfos.push_back(VkDeviceQueueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = queueFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority});
  }

  VkPhysicalDeviceFeatures deviceFeatures{.multiViewport = VK_TRUE,
                                          .samplerAnisotropy = VK_TRUE};

  VkDeviceCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
      .ppEnabledExtensionNames = extensionNames.data(),
      .pEnabledFeatures = &deviceFeatures};

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

  VkCommandPoolCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
               VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = graphics_queue_family};

  if (vkCreateCommandPool(device, &createInfo, nullptr, &command_pool) !=
      VK_SUCCESS) {
    log_ftl("Failed to create Vulkan command pool.");
  }
}

void GpuInstance::createAllocator() {
  log_zone;

  VmaAllocatorCreateInfo createInfo{.physicalDevice = physical_device,
                                    .device = device,
                                    .instance = instance};

  if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
    log_ftl("Failed to create Vulkan memory allocator.");
  }
}

}  // namespace mondradiko
