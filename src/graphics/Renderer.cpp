#include "graphics/Renderer.hpp"

#include "graphics/CameraDescriptorSet.hpp"
#include "graphics/FrameData.hpp"
#include "graphics/Viewport.hpp"
#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "xr/Session.hpp"

Renderer::Renderer(VulkanInstance* _vulkanInstance, Session* _session)
{
    log_dbg("Creating renderer.");

    vulkanInstance = _vulkanInstance;
    session = _session;

    findSwapchainFormat();
    createRenderPasses();    

    if(!session->createViewports(&viewports, swapchainFormat, compositePass)) {
        log_ftl("Failed to create Renderer viewports.");
    }

    cameraDescriptorSet = new CameraDescriptorSet(vulkanInstance, viewports.size());
    frameData = new FrameData(vulkanInstance, 2); // Pipeline two frames

    createPipelines();
}

Renderer::~Renderer()
{
    log_dbg("Destroying renderer.");

    vkDeviceWaitIdle(vulkanInstance->device);

    if(cameraDescriptorSet != nullptr) delete cameraDescriptorSet;
    if(frameData != nullptr) delete frameData;
    if(meshPipeline != nullptr) delete meshPipeline;

    for(Viewport* viewport : viewports) {
        delete viewport;
    }

    if(compositePass != VK_NULL_HANDLE) vkDestroyRenderPass(vulkanInstance->device, compositePass, nullptr);
}

void Renderer::renderFrame()
{
    for(uint32_t viewportIndex = 0; viewportIndex < viewports.size(); viewportIndex++) {
        viewports[viewportIndex]->acquireSwapchainImage();
    }

    VkCommandBuffer commandBuffer = frameData->beginPrimaryCommand();

    for(uint32_t viewportIndex = 0; viewportIndex < viewports.size(); viewportIndex++) {
        viewports[viewportIndex]->beginRenderPass(commandBuffer, compositePass);
        viewports[viewportIndex]->setCommandViewport(commandBuffer);
        cameraDescriptorSet->bind(commandBuffer, viewportIndex, meshPipeline->pipelineLayout);
        meshPipeline->render(commandBuffer);
        vkCmdEndRenderPass(commandBuffer);
    }

    frameData->endPrimaryCommand();

    for(uint32_t viewportIndex = 0; viewportIndex < viewports.size(); viewportIndex++) {
        viewports[viewportIndex]->releaseSwapchainImage();
    }
}

void Renderer::finishRender(std::vector<XrView>* views, std::vector<XrCompositionLayerProjectionView>* projectionViews)
{
    cameraDescriptorSet->update(views);
    frameData->submitPrimaryCommand();

    for(uint32_t i = 0; i < views->size(); i++) {
        (*projectionViews)[i] = {
            .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW,
            .pose = (*views)[i].pose,
            .fov = (*views)[i].fov,
            .subImage = {
                viewports[i]->swapchain,
                .imageRect = {
                    .offset = { 0, 0 },
                    .extent = { (int32_t) viewports[i]->width, (int32_t) viewports[i]->height }
                }
            }
        };
    }
}

void Renderer::findSwapchainFormat()
{
    std::vector<VkFormat> formatOptions;
    session->enumerateSwapchainFormats(&formatOptions);

    std::vector<VkFormat> formatCandidates = {
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_R8G8B8A8_UNORM
    };

    if(!vulkanInstance->findFormatFromOptions(&formatOptions, &formatCandidates, &swapchainFormat)) {
        log_ftl("Failed to find suitable swapchain format.");
    }
}

void Renderer::createRenderPasses()
{
    VkAttachmentDescription swapchainAttachmentDescription{
        .format = swapchainFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference swapchainTargetReference{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    std::vector<VkAttachmentDescription> attachments = {
        swapchainAttachmentDescription
    };

    VkSubpassDescription compositeSubpassDescription{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &swapchainTargetReference
    };

    VkRenderPassCreateInfo compositePassCreateInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = (uint32_t) attachments.size(),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &compositeSubpassDescription
    };

    if(vkCreateRenderPass(vulkanInstance->device, &compositePassCreateInfo, nullptr, &compositePass) != VK_SUCCESS) {
        log_ftl("Failed to create Renderer composite render pass.");
    }
}

void Renderer::createPipelines()
{
    meshPipeline = new MeshPipeline(vulkanInstance, cameraDescriptorSet->setLayout, compositePass, 0);
}
