#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class VulkanInstance;
class Viewport;

struct VulkanRequirements
{
    uint32_t minApiVersion;
    uint32_t maxApiVersion;

    std::vector<std::string> instanceExtensions;

    VkPhysicalDevice physicalDevice;

    std::vector<std::string> deviceExtensions;
};

class XrDisplay
{
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

    PFN_xrCreateDebugUtilsMessengerEXT ext_xrCreateDebugUtilsMessengerEXT = nullptr;
    PFN_xrDestroyDebugUtilsMessengerEXT ext_xrDestroyDebugUtilsMessengerEXT = nullptr;
    PFN_xrGetVulkanGraphicsRequirementsKHR ext_xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR ext_xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR ext_xrGetVulkanGraphicsDeviceKHR = nullptr;
    PFN_xrGetVulkanDeviceExtensionsKHR ext_xrGetVulkanDeviceExtensionsKHR = nullptr;
};
