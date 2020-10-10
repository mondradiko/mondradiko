#include "graphics/pipelines/MeshPipeline.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"

MeshPipeline::MeshPipeline(VulkanInstance* _vulkanInstance)
{
    log_dbg("Creating mesh pipeline.");

    vulkanInstance = _vulkanInstance;
}

MeshPipeline::~MeshPipeline()
{
    log_dbg("Destroying mesh pipeline.");

    if(pipeline != VK_NULL_HANDLE) vkDestroyPipeline(vulkanInstance->device, pipeline, nullptr);
    if(pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(vulkanInstance->device, pipelineLayout, nullptr);
}

void MeshPipeline::render(VkCommandBuffer commandBuffer)
{
    //vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    //render(); lol
}
