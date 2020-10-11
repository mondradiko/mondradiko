#include "assets/MeshAsset.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"

MeshAsset::MeshAsset(std::string meshName, VulkanInstance* vulkanInstance, aiMesh* mesh)
 : meshName(meshName),
   vulkanInstance(vulkanInstance)
{
    log_dbg("Loading mesh asset with name:");
    log_dbg(meshName.c_str());

    if((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) != aiPrimitiveType_TRIANGLE) {
        log_ftl("Mesh topology is not exclusively triangles.");
    }

    std::vector<MeshVertex> vertices(mesh->mNumVertices);

    for(uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
        aiVector3D position = mesh->mVertices[vertexIndex];
        aiColor4D color = mesh->mColors[vertexIndex][0];

        vertices[vertexIndex].position = glm::vec3(position.x, position.y, position.z);
        vertices[vertexIndex].color = glm::vec3(color.r, color.g, color.b);
    }

    // Three indices per triangle face
    std::vector<MeshIndex> indices(mesh->mNumFaces * 3);

    uint32_t indexIndex = 0;
    for(uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
        aiFace& face = mesh->mFaces[faceIndex];

        for(uint32_t faceCornerIndex = 0; faceCornerIndex < face.mNumIndices; faceCornerIndex++) {
            indices[indexIndex] = face.mIndices[faceCornerIndex];
            indexIndex++;
        }
    }
}

MeshAsset::~MeshAsset()
{
    log_dbg("Destroying mesh asset.");
    if(indexAllocation != nullptr) vmaDestroyBuffer(vulkanInstance->allocator, indexBuffer, indexAllocation);
    if(vertexAllocation != nullptr) vmaDestroyBuffer(vulkanInstance->allocator, vertexBuffer, vertexAllocation);
}
