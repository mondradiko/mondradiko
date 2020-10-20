#pragma once

#include <map>
#include <set>
#include <string>

#include "api_headers.hpp"
#include "assets/AssetHandle.hpp"
#include "assets/AssetPool.hpp"

class MaterialAsset;
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

    MeshRendererComponent* createMeshRenderer(std::string, const aiScene*, uint32_t);

    AssetHandle<MaterialAsset> loadMaterial(std::string, const aiScene*, uint32_t);
    AssetHandle<MeshAsset> loadMesh(std::string, const aiScene*, uint32_t);
    AssetHandle<TextureAsset> loadTexture(std::string, const aiScene*, aiString);

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    std::set<MeshRendererComponent*> meshRenderers;
private:
    VulkanInstance* vulkanInstance;

    AssetPool<MaterialAsset> materialAssets;
    AssetPool<MeshAsset> meshAssets;
    AssetPool<TextureAsset> textureAssets;

    void createPipelineLayout(VkDescriptorSetLayout);
    void createPipeline(VkRenderPass, uint32_t);
};
