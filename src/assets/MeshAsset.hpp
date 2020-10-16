#pragma once

#include <array>
#include <string>

#include "api_headers.hpp"

class VulkanBuffer;
class VulkanInstance;

using MeshVertexAttributeDescriptions = std::array<VkVertexInputAttributeDescription, 3>;

struct MeshVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription description{
            .binding = 0,
            .stride = sizeof(MeshVertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };

        return description;
    }

    static MeshVertexAttributeDescriptions getAttributeDescriptions() {
       MeshVertexAttributeDescriptions descriptions;

       descriptions[0] = {
           .location = 0,
           .binding = 0,
           .format = VK_FORMAT_R32G32B32_SFLOAT,
           .offset = offsetof(MeshVertex, position)
       };

       descriptions[1] = {
           .location = 1,
           .binding = 0,
           .format = VK_FORMAT_R32G32B32_SFLOAT,
           .offset = offsetof(MeshVertex, normal)
       };

       descriptions[2] = {
           .location = 2,
           .binding = 0,
           .format = VK_FORMAT_R32G32_SFLOAT,
           .offset = offsetof(MeshVertex, texCoord)
       };

       return descriptions;
    }
};

using MeshIndex = uint32_t;

class MeshAsset
{
public:
    MeshAsset(std::string, VulkanInstance*, aiMesh*);
    ~MeshAsset();

    VulkanBuffer* vertexBuffer = nullptr;
    VulkanBuffer* indexBuffer = nullptr;
    size_t indexCount = 0;

    std::string meshName = nullptr;
private:
    VulkanInstance* vulkanInstance;
};
