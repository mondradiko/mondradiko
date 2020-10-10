#pragma once

#include "api_headers.hpp"

class VulkanInstance;

class MeshPipeline
{
public:
    MeshPipeline(VulkanInstance*);
    ~MeshPipeline();

    void render(VkCommandBuffer);
private:
    VulkanInstance* vulkanInstance;

    void createPipelineLayout();
    void createPipeline();

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
};
