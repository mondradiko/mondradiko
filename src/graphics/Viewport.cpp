#include "graphics/Viewport.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "xr/Session.hpp"

bool Viewport::initialize(VkFormat format, VkRenderPass renderPass, XrViewConfigurationView* viewConfig, Session* _session, VulkanInstance* _vulkanInstance)
{
    log_dbg("Creating viewport.");

    session = _session;
    vulkanInstance = _vulkanInstance;

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

    if(xrCreateSwapchain(session->session, &swapchainCreateInfo, &swapchain) != XR_SUCCESS) {
        log_err("Failed to create swapchain.");
        return false;
    }

    uint32_t imageCount;
    xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
    std::vector<XrSwapchainImageVulkanKHR> vulkanImages(imageCount);
    xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount, (XrSwapchainImageBaseHeader*) vulkanImages.data());

    images.resize(imageCount);
    for(uint32_t i = 0; i < imageCount; i++) {
        images[i].image = vulkanImages[i].image;

        VkImageViewCreateInfo viewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = images[i].image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if(vkCreateImageView(vulkanInstance->device, &viewCreateInfo, nullptr, &images[i].imageView) != VK_SUCCESS) {
            log_err("Failed to create viewport image view.");
            return false;
        }

        VkFramebufferCreateInfo framebufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = &images[i].imageView,
            .width = viewConfig->recommendedImageRectWidth,
            .height = viewConfig->recommendedImageRectHeight,
            .layers = 1
        };

        if(vkCreateFramebuffer(vulkanInstance->device, &framebufferCreateInfo, nullptr, &images[i].framebuffer) != VK_SUCCESS) {
            log_err("Failed to create viewport framebuffer.");
            return false;
        }
    }

    return true;
}

void Viewport::destroy()
{
    log_dbg("Destroying viewport.");
    
    for(uint32_t i = 0; i < images.size(); i++) {
        vkDestroyFramebuffer(vulkanInstance->device, images[i].framebuffer, nullptr);
        vkDestroyImageView(vulkanInstance->device, images[i].imageView, nullptr);
    }

    if(swapchain != XR_NULL_HANDLE) xrDestroySwapchain(swapchain);
}
