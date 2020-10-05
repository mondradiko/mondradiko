#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

struct RendererRequirements
{
    uint32_t minApiVersion;
    uint32_t maxApiVersion;

    std::vector<std::string> instanceExtensions;

    VkPhysicalDevice physicalDevice;
};

class XrDisplay
{
public:
    ~XrDisplay();

    bool initialize();
    bool getRequirements(RendererRequirements*);
    bool getVulkanDevice(VkInstance, VkPhysicalDevice*);
    bool createSession(class Renderer*);

    bool shouldQuit = false;
private:
    bool createInstance();
    bool findSystem();

    PFN_xrGetVulkanGraphicsRequirementsKHR ext_xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR ext_xrGetVulkanInstanceExtensionsKHR = nullptr;
    PFN_xrGetVulkanGraphicsDeviceKHR ext_xrGetVulkanGraphicsDeviceKHR = nullptr;
    
    XrInstance instance = XR_NULL_HANDLE;
    XrSystemId systemId = XR_NULL_SYSTEM_ID;
    XrSession session = XR_NULL_HANDLE;
};
