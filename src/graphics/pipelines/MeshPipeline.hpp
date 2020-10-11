#pragma once

#include "api_headers.hpp"

class VulkanInstance;

class MeshPipeline
{
public:
    MeshPipeline(VulkanInstance*, VkDescriptorSetLayout, VkRenderPass, uint32_t);
    ~MeshPipeline();

    void render(VkCommandBuffer);

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;

    void createPipelineLayout(VkDescriptorSetLayout);
    void createPipeline(VkRenderPass, uint32_t);
};
