#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class XrDisplay;
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
    bool initialize(VkFormat, XrViewConfigurationView*, XrDisplay*, VulkanInstance*);
    void destroy();

    XrSwapchain swapchain = XR_NULL_HANDLE;
    std::vector<ViewportImage> images;
private:
};
