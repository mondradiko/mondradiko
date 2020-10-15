#include "assets/MeshAsset.hpp"

#include "graphics/VulkanBuffer.hpp"
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

    if(!mesh->HasPositions()) {
        log_ftl("Mesh vertices have no positions.");
    }

    if(!mesh->HasNormals()) {
        log_ftl("Mesh vertices have no normals.");
    }

    std::vector<MeshVertex> vertices(mesh->mNumVertices);

    for(uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
        aiVector3D position = mesh->mVertices[vertexIndex];
        vertices[vertexIndex].position = glm::vec3(position.x, position.y, position.z);
    }

    for(uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
        aiVector3D normal = mesh->mNormals[vertexIndex];
        vertices[vertexIndex].normal = glm::vec3(normal.x, normal.y, normal.z);
    }

    aiColor4D vertexColor = aiColor4D(1.0, 1.0, 1.0, 1.0);
    for(uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
        if(mesh->HasVertexColors(0)) {
            vertexColor = mesh->mColors[0][vertexIndex];
        }

        vertices[vertexIndex].color = glm::vec3(vertexColor.r, vertexColor.g, vertexColor.b);
    }

    // Three indices per triangle face
    indexCount = mesh->mNumFaces * 3;
    std::vector<MeshIndex> indices(indexCount);

    uint32_t indexIndex = 0;
    for(uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
        aiFace& face = mesh->mFaces[faceIndex];

        for(uint32_t faceCornerIndex = 0; faceCornerIndex < face.mNumIndices; faceCornerIndex++) {
            indices[indexIndex] = face.mIndices[faceCornerIndex];
            indexIndex++;
        }
    }

    size_t vertexSize = sizeof(MeshVertex) * vertices.size();
    vertexBuffer = new VulkanBuffer(vulkanInstance, vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    vertexBuffer->writeData(vertices.data());

    size_t indexSize = sizeof(indices[0]) * indices.size();
    indexBuffer = new VulkanBuffer(vulkanInstance, indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    indexBuffer->writeData(indices.data());
}

MeshAsset::~MeshAsset()
{
    log_dbg("Destroying mesh asset.");
    
    if(vertexBuffer != nullptr) delete vertexBuffer;
    if(indexBuffer != nullptr) delete indexBuffer;
}
