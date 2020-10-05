#pragma once

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class XrDisplay
{
public:
    ~XrDisplay();

    bool initialize();
    bool createSession(class Renderer*);

    bool shouldQuit = false;
private:
    bool createInstance();
    bool findSystem();
    
    XrInstance instance = XR_NULL_HANDLE;
    XrSystemId systemId = XR_NULL_SYSTEM_ID;
    XrSession session = XR_NULL_HANDLE;
};
