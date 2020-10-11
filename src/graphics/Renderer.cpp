#include "graphics/Renderer.hpp"

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
    createCameraDescriptor();
    createPipelines();

    if(!session->createViewports(&viewports, swapchainFormat, compositePass)) {
        log_ftl("Failed to create Renderer viewports.");
    }
}

Renderer::~Renderer()
{
    log_dbg("Destroying renderer.");

    if(meshPipeline != nullptr) delete meshPipeline;

    for(Viewport* viewport : viewports) {
        delete viewport;
    }

    if(compositePass != VK_NULL_HANDLE) vkDestroyRenderPass(vulkanInstance->device, compositePass, nullptr);

    if(cameraAllocation != nullptr) vmaDestroyBuffer(vulkanInstance->allocator, cameraBuffer, cameraAllocation);

    if(cameraSetLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(vulkanInstance->device, cameraSetLayout, nullptr);
}

void Renderer::renderFrame()
{
    CameraUniform ubo;
    ubo.view = glm::mat4(1.0);
    ubo.projection = glm::mat4(1.0);

    void* data;
    vkMapMemory(vulkanInstance->device, cameraAllocationInfo.deviceMemory, cameraAllocationInfo.offset, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vulkanInstance->device, cameraAllocationInfo.deviceMemory);

    for(uint32_t viewportIndex = 0; viewportIndex < viewports.size(); viewportIndex++) {
        VkCommandBuffer commandBuffer;
        VkFramebuffer framebuffer;
        viewports[viewportIndex]->acquireSwapchainImage(&commandBuffer, &framebuffer);

        viewports[viewportIndex]->setCommandViewport(commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline->pipelineLayout, 0, 1, &cameraSet, 0, nullptr);

        viewports[viewportIndex]->beginRenderPass(commandBuffer, framebuffer, compositePass);
        meshPipeline->render(commandBuffer);
        vkCmdEndRenderPass(commandBuffer);

        viewports[viewportIndex]->releaseSwapchainImage(commandBuffer);
    }
}

void Renderer::finishRender(std::vector<XrView>* views, std::vector<XrCompositionLayerProjectionView>* projectionViews)
{
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

void Renderer::createCameraDescriptor()
{
    VkDescriptorSetLayoutBinding uboBinding{
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo setLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &uboBinding
    };

    if(vkCreateDescriptorSetLayout(vulkanInstance->device, &setLayoutInfo, nullptr, &cameraSetLayout) != VK_SUCCESS) {
        log_ftl("Failed to create camera descriptor set layout.");
    }

    VkDescriptorSetAllocateInfo setInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vulkanInstance->descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &cameraSetLayout
    };

    if(vkAllocateDescriptorSets(vulkanInstance->device, &setInfo, &cameraSet) != VK_SUCCESS) {
        log_ftl("Failed to allocate camera descriptor set.");
    }

    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(CameraUniform),
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo allocationInfo{
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU
    };

    if(vmaCreateBuffer(vulkanInstance->allocator, &bufferInfo, &allocationInfo, &cameraBuffer, &cameraAllocation, &cameraAllocationInfo) != VK_SUCCESS) {
        log_ftl("Failed to allocate camera uniform buffer.");
    }

    VkDescriptorBufferInfo bufferDescriptorInfo{
        .buffer = cameraBuffer,
        .offset = 0,
        .range = sizeof(CameraUniform)
    };

    VkWriteDescriptorSet descriptorWrite{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = cameraSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferDescriptorInfo
    };

    vkUpdateDescriptorSets(vulkanInstance->device, 1, &descriptorWrite, 0, nullptr);
}

void Renderer::createPipelines()
{
    meshPipeline = new MeshPipeline(vulkanInstance, cameraSetLayout, compositePass, 0);
}
