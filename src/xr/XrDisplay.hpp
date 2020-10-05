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
};

class XrDisplay
{
public:
    ~XrDisplay();

    bool initialize(RendererRequirements*);
    bool createSession(class Renderer*);

    bool shouldQuit = false;
private:
    bool createInstance();
    bool findSystem();
    bool getRequirements(RendererRequirements*);

    PFN_xrGetVulkanGraphicsRequirementsKHR ext_xrGetVulkanGraphicsRequirementsKHR = nullptr;
    PFN_xrGetVulkanInstanceExtensionsKHR ext_xrGetVulkanInstanceExtensionsKHR = nullptr;
    
    XrInstance instance = XR_NULL_HANDLE;
    XrSystemId systemId = XR_NULL_SYSTEM_ID;
    XrSession session = XR_NULL_HANDLE;
};
