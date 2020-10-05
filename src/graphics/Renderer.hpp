#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class XrDisplay;
struct RendererRequirements;

class Renderer
{
public:
    ~Renderer();

    bool initialize(XrDisplay*);

    bool enableValidationLayers = true;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    uint32_t graphicsQueueFamily;
private:
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool checkValidationLayerSupport();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT*);
    bool createInstance(RendererRequirements*);
    bool setupDebugMessenger();
    bool findPhysicalDevice(XrDisplay*);
    bool findQueueFamilies();
    bool createLogicalDevice(RendererRequirements*);

    PFN_vkCreateDebugUtilsMessengerEXT ext_vkCreateDebugUtilsMessengerEXT = nullptr;
    PFN_vkDestroyDebugUtilsMessengerEXT ext_vkDestroyDebugUtilsMessengerEXT = nullptr;
};
