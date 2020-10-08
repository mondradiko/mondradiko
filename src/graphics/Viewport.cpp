#include "graphics/Viewport.hpp"

#include <array>

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "xr/Session.hpp"

bool Viewport::initialize(VkFormat format, VkRenderPass renderPass, XrViewConfigurationView* viewConfig, Session* _session, VulkanInstance* _vulkanInstance)
{
    log_dbg("Creating viewport.");

    session = _session;
    vulkanInstance = _vulkanInstance;

    width = viewConfig->recommendedImageRectWidth;
    height = viewConfig->recommendedImageRectHeight;

    XrSwapchainCreateInfo swapchainCreateInfo{
        .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
        .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
        .format = (int64_t) format,
        .sampleCount = 1,
        .width = width,
        .height = height,
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
            .width = width,
            .height = height,
            .layers = 1
        };

        if(vkCreateFramebuffer(vulkanInstance->device, &framebufferCreateInfo, nullptr, &images[i].framebuffer) != VK_SUCCESS) {
            log_err("Failed to create viewport framebuffer.");
            return false;
        }

        VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = vulkanInstance->commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if(vkAllocateCommandBuffers(vulkanInstance->device, &allocInfo, &images[i].commandBuffer) != VK_SUCCESS) {
            log_err("Failed to allocate viewport command buffer.");
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
        vkFreeCommandBuffers(vulkanInstance->device, vulkanInstance->commandPool, 1, &images[i].commandBuffer);
    }

    if(swapchain != XR_NULL_HANDLE) xrDestroySwapchain(swapchain);
}

void Viewport::acquireSwapchainImage(VkCommandBuffer* commandBuffer, VkFramebuffer* framebuffer)
{
    XrSwapchainImageAcquireInfo acquireInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO,
        .next = nullptr
    };

    uint32_t index;
    xrAcquireSwapchainImage(swapchain, &acquireInfo, &index);

    XrSwapchainImageWaitInfo waitInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
        .timeout = XR_INFINITE_DURATION
    };

    xrWaitSwapchainImage(swapchain, &waitInfo);

    *framebuffer = images[index].framebuffer;
    *commandBuffer = images[index].commandBuffer;

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(*commandBuffer, &beginInfo);
}

void Viewport::beginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkRenderPass renderPass)
{
    std::array<VkClearValue, 1> clearValues;

    clearValues[0].color = {0.2, 0.0, 0.0, 1.0};

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea = {
            .offset = {0, 0},
            .extent = {width, height}
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Viewport::releaseSwapchainImage(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if(vkQueueSubmit(vulkanInstance->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        log_ftl("Failed to submit draw command buffer!");
    }

    vkQueueWaitIdle(vulkanInstance->graphicsQueue);

    XrSwapchainImageReleaseInfo releaseInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO,
        .next = nullptr
    };

    xrReleaseSwapchainImage(swapchain, &releaseInfo);
}
