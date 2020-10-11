#pragma once

#include <vector>

#include "api_headers.hpp"

class VulkanInstance;

class FrameData
{
public:
    FrameData(VulkanInstance*, uint32_t);
    ~FrameData();

    VkCommandBuffer beginPrimaryCommand();
    void endPrimaryCommand();
    void submitPrimaryCommand();
private:
    VulkanInstance* vulkanInstance;

    uint32_t framesInFlightCount;
    uint32_t currentFrame = 0;

    std::vector<VkCommandBuffer> commandBuffers;
};
