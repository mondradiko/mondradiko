#include "graphics/Renderer.hpp"

#include "graphics/Viewport.hpp"
#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "xr/Session.hpp"

Renderer::Renderer(VulkanInstance* vulkanInstance, Session* session)
{
    log_dbg("Creating renderer.");

    std::vector<VkFormat> formatOptions;
    session->enumerateSwapchainFormats(&formatOptions);

    std::vector<VkFormat> formatCandidates = {
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM
    };

    VkFormat swapchainFormat;
    if(!vulkanInstance->findFormatFromOptions(&formatOptions, &formatCandidates, &swapchainFormat)) {
        log_err("Failed to find suitable swapchain format.");
    }

    session->createViewports(&viewports, swapchainFormat, VK_NULL_HANDLE);
}

Renderer::~Renderer()
{
    log_dbg("Destroying renderer.");

    for(uint32_t i = 0; i < viewports.size(); i++) {
        viewports[i].destroy();
    }
}
