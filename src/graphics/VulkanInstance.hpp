#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/Viewport.hpp"

class XrDisplay;
struct VulkanRequirements;

class VulkanInstance
{
public:
    VulkanInstance(XrDisplay*);
    ~VulkanInstance();

    bool findFormatFromOptions(const std::vector<VkFormat>*, const std::vector<VkFormat>*, VkFormat*);

    bool enableValidationLayers = true;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    uint32_t graphicsQueueFamily;
    VkQueue graphicsQueue;

    std::vector<Viewport> viewports;
private:
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool checkValidationLayerSupport();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT*);
    void createInstance(VulkanRequirements*);
    void setupDebugMessenger();
    void findPhysicalDevice(XrDisplay*);
    void findQueueFamilies();
    void createLogicalDevice(VulkanRequirements*);

    PFN_vkCreateDebugUtilsMessengerEXT ext_vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT ext_vkDestroyDebugUtilsMessengerEXT = nullptr;
};