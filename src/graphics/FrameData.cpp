#include "graphics/FrameData.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"

FrameData::FrameData(VulkanInstance* vulkanInstance, uint32_t framesInFlightCount)
 : vulkanInstance(vulkanInstance),
   framesInFlightCount(framesInFlightCount)
{
    log_dbg("Creating frame data.");

    commandBuffers.resize(framesInFlightCount);

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vulkanInstance->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = framesInFlightCount
    };

    if(vkAllocateCommandBuffers(vulkanInstance->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        log_ftl("Failed to allocate frame command buffers.");
    }
}

FrameData::~FrameData()
{
    log_dbg("Destroying frame data.");

    if(commandBuffers.size() > 0) {
        vkFreeCommandBuffers(vulkanInstance->device, vulkanInstance->commandPool, commandBuffers.size(), commandBuffers.data());
    }
}

VkCommandBuffer FrameData::beginPrimaryCommand()
{
    if(++currentFrame >= framesInFlightCount) {
        currentFrame = 0;
    }

    VkCommandBuffer commandBuffer = commandBuffers[currentFrame];

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void FrameData::endPrimaryCommand()
{
    vkEndCommandBuffer(commandBuffers[currentFrame]);
}

void FrameData::submitPrimaryCommand()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if(vkQueueSubmit(vulkanInstance->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        log_ftl("Failed to submit primary frame command buffer.");
    }
}
