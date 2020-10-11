#pragma once

#include <string>

#include "api_headers.hpp"

class VulkanInstance;

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

using MeshIndex = uint32_t;

class MeshAsset
{
public:
    MeshAsset(std::string, VulkanInstance*, aiMesh*);
    ~MeshAsset();

    VmaAllocation vertexAllocation = nullptr;
    VmaAllocation indexAllocation = nullptr;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;

    std::string meshName = nullptr;
private:
    VulkanInstance* vulkanInstance;
};
