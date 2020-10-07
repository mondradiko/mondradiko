#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "graphics/Viewport.hpp"

class XrDisplay;
struct RendererRequirements;

class VulkanInstance
{
public:
    ~VulkanInstance();

    bool initialize(XrDisplay*);
    bool prepareRender(XrDisplay*);

    bool findFormatFromOptions(const std::vector<VkFormat>*, const std::vector<VkFormat>*, VkFormat*);

    bool enableValidationLayers = true;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    uint32_t graphicsQueueFamily;

    std::vector<Viewport> viewports;
private:
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // initialize() steps
    bool checkValidationLayerSupport();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT*);
    bool createInstance(RendererRequirements*);
    bool setupDebugMessenger();
    bool findPhysicalDevice(XrDisplay*);
    bool findQueueFamilies();
    bool createLogicalDevice(RendererRequirements*);

    // prepareRender() steps
    bool createViewports(XrDisplay*);

    PFN_vkCreateDebugUtilsMessengerEXT ext_vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT ext_vkDestroyDebugUtilsMessengerEXT = nullptr;
};
