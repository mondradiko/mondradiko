#include "graphics/Viewport.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "xr/XrDisplay.hpp"

bool Viewport::initialize(VkFormat format, XrViewConfigurationView* viewConfig, XrDisplay* display, VulkanInstance* renderer)
{
    log_dbg("Creating viewport.");

    XrSwapchainCreateInfo swapchainCreateInfo{
        .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
        .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
        .format = (int64_t) format,
        .sampleCount = 1,
        .width = viewConfig->recommendedImageRectWidth,
        .height = viewConfig->recommendedImageRectHeight,
        .faceCount = 1,
        .arraySize = 1,
        .mipCount = 1
    };

    if(xrCreateSwapchain(display->session, &swapchainCreateInfo, &swapchain) != XR_SUCCESS) {
        log_err("Failed to create swapchain.");
        return false;
    }

    return true;
}

void Viewport::destroy()
{
    log_dbg("Destroying viewport.");

    if(swapchain != XR_NULL_HANDLE) xrDestroySwapchain(swapchain);
}
