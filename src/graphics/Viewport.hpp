#pragma once

#include <vector>

#include "api_headers.hpp"

class Session;
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
    Viewport(VkFormat, VkRenderPass, XrViewConfigurationView*, Session*, VulkanInstance*);
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
    Session* session;
    VulkanInstance* vulkanInstance;

    uint32_t currentIndex;
};
