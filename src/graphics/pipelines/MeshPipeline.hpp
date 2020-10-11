#pragma once

#include <map>
#include <string>

#include "api_headers.hpp"

class MeshAsset;
class VulkanInstance;

class MeshPipeline
{
public:
    MeshPipeline(VulkanInstance*, VkDescriptorSetLayout, VkRenderPass, uint32_t);
    ~MeshPipeline();

    void render(VkCommandBuffer);

    MeshAsset* loadMesh(std::string, aiMesh*);

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;

    std::map<std::string, MeshAsset*> meshAssets;

    void createPipelineLayout(VkDescriptorSetLayout);
    void createPipeline(VkRenderPass, uint32_t);
};
