#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

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
    void releaseSwapchainImage(VkCommandBuffer);

    XrSwapchain swapchain = XR_NULL_HANDLE;
    std::vector<ViewportImage> images;

    uint32_t width;
    uint32_t height;
private:
    Session* session;
    VulkanInstance* vulkanInstance;
};
