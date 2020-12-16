/**
 * @file GpuInstance.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Manages all low-level Vulkan objects such as device, debug messenger,
 * VMA allocator, etc.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations
class DisplayInterface;
struct VulkanRequirements;

class GpuInstance {
 public:
  explicit GpuInstance(DisplayInterface*);
  ~GpuInstance();

  bool findFormatFromOptions(const std::vector<VkFormat>*,
                             const std::vector<VkFormat>*, VkFormat*);
  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer);

  bool enableValidationLayers = true;

  DisplayInterface* display;

  VkInstance instance = VK_NULL_HANDLE;
  VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
  VkPhysicalDevice physical_device = VK_NULL_HANDLE;
  VkDevice device = VK_NULL_HANDLE;

  uint32_t graphics_queue_family;
  VkQueue graphics_queue;

  VkCommandPool command_pool = VK_NULL_HANDLE;

  VmaAllocator allocator = nullptr;

 private:
  const std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};

  bool checkValidationLayerSupport();
  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT*);
  void createInstance(VulkanRequirements*);
  void setupDebugMessenger();
  void findPhysicalDevice(DisplayInterface*);
  void findQueueFamilies();
  void createLogicalDevice(VulkanRequirements*);
  void createCommandPool();
  void createAllocator();

  PFN_vkCreateDebugUtilsMessengerEXT ext_vkCreateDebugUtilsMessengerEXT =
      nullptr;
  PFN_vkDestroyDebugUtilsMessengerEXT ext_vkDestroyDebugUtilsMessengerEXT =
      nullptr;
};

}  // namespace mondradiko
