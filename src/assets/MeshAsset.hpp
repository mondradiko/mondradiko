#pragma once

#include <string>

#include "api_headers.hpp"

class VulkanBuffer;
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

    VulkanBuffer* vertexBuffer = nullptr;
    VulkanBuffer* indexBuffer = nullptr;

    std::string meshName = nullptr;
private:
    VulkanInstance* vulkanInstance;
};
