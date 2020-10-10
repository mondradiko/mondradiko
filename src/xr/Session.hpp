#pragma once

#include <vector>

#include "api_headers.hpp"

class Renderer;
class XrDisplay;
class Viewport;
class VulkanInstance;

class Session
{
public:
    Session(XrDisplay*, VulkanInstance*);
    ~Session();

    void pollEvents(bool*, bool*);
    void beginFrame(double*, bool*);
    void endFrame(Renderer*, bool);

    void enumerateSwapchainFormats(std::vector<VkFormat>*);
    bool createViewports(std::vector<Viewport>*, VkFormat, VkRenderPass);

    XrSession session = XR_NULL_HANDLE;
    XrSessionState sessionState = XR_SESSION_STATE_UNKNOWN;
    XrFrameState currentFrameState;

    XrSpace stageSpace = XR_NULL_HANDLE;

    std::vector<XrView> views;
private:
    XrDisplay* display;
    VulkanInstance* vulkanInstance;
};
