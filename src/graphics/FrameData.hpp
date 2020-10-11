#pragma once

#include <vector>

#include "api_headers.hpp"

class VulkanInstance;

struct FrameInFlight
{
    VkCommandBuffer commandBuffer;
    VkFence isInUse;
    bool submitted;
};

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

    uint32_t currentFrame = 0;
    std::vector<FrameInFlight> framesInFlight;
};
