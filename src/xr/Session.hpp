#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class XrDisplay;
class VulkanInstance;

class Session
{
public:
    Session(XrDisplay*, VulkanInstance*);
    ~Session();

    void pollEvents(bool*, bool*);
    void beginFrame(double*, bool*);
    void endFrame();

    void enumerateSwapchainFormats(std::vector<VkFormat>*);

    XrSession session = XR_NULL_HANDLE;
    XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
    XrFrameState currentFrameState;
private:
    XrDisplay* display;
};
