#pragma once

#include <vector>

#include "api_headers.h"

class PlayerSession;
class VulkanInstance;

struct ViewportImage
{
    VkImage image;
    VkImageView imageView;
    VkFramebuffer framebuffer;
};

class Viewport
{
public:
    Viewport(VkFormat, VkRenderPass, XrViewConfigurationView*, PlayerSession*, VulkanInstance*);
    ~Viewport();

    void acquireSwapchainImage();
    void beginRenderPass(VkCommandBuffer, VkRenderPass);
    void setCommandViewport(VkCommandBuffer);
    void releaseSwapchainImage();

    XrSwapchain swapchain = XR_NULL_HANDLE;
    std::vector<ViewportImage> images;

    uint32_t width;
    uint32_t height;
private:
    PlayerSession* session;
    VulkanInstance* vulkanInstance;

    uint32_t currentIndex;
};
