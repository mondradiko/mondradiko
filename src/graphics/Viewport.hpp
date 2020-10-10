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
    VkCommandBuffer commandBuffer;
};

class Viewport
{
public:
    bool initialize(VkFormat, VkRenderPass, XrViewConfigurationView*, Session*, VulkanInstance*);
    void destroy();

    void acquireSwapchainImage(VkCommandBuffer*, VkFramebuffer*);
    void beginRenderPass(VkCommandBuffer, VkFramebuffer, VkRenderPass);
    void releaseSwapchainImage(VkCommandBuffer);

    XrSwapchain swapchain = XR_NULL_HANDLE;
    std::vector<ViewportImage> images;

    uint32_t width;
    uint32_t height;
private:
    Session* session;
    VulkanInstance* vulkanInstance;
};
