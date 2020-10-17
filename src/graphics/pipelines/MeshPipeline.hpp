#pragma once

#include <map>
#include <string>

#include "api_headers.hpp"

class MeshAsset;
class MeshRendererComponent;
class TextureAsset;
class VulkanInstance;

class MeshPipeline
{
public:
    MeshPipeline(VulkanInstance*, VkDescriptorSetLayout, VkRenderPass, uint32_t);
    ~MeshPipeline();

    void render(VkCommandBuffer);

    MeshRendererComponent* createMeshRenderer(const aiScene*, aiMesh*);

    MeshAsset* loadMesh(std::string, aiMesh*);
    TextureAsset* loadTexture(std::string, aiTexture*, uint32_t);

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;

    std::map<std::string, MeshAsset*> meshAssets;
    std::map<std::string, TextureAsset*> textureAssets;

    void createPipelineLayout(VkDescriptorSetLayout);
    void createPipeline(VkRenderPass, uint32_t);
};
