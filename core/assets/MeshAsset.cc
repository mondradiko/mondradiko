/**
 * @file MeshAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains the geometry for a mesh.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/MeshAsset.h"

#include <vector>

#include "core/log/log.h"

namespace mondradiko {

MeshAsset::MeshAsset(std::string mesh_name, GpuInstance* gpu, aiMesh* mesh)
    : mesh_name(mesh_name), gpu(gpu) {
  log_dbg("Loading mesh asset with name:");
  log_dbg(mesh_name.c_str());

  if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) !=
      aiPrimitiveType_TRIANGLE) {
    log_ftl("Mesh topology is not exclusively triangles.");
  }

  if (!mesh->HasPositions()) {
    log_ftl("Mesh vertices have no positions.");
  }

  if (!mesh->HasNormals()) {
    log_ftl("Mesh vertices have no normals.");
  }

  if (!mesh->HasTextureCoords(0)) {
    log_ftl("Mesh vertices have no texture coordinates.");
  }

  std::vector<MeshVertex> vertices(mesh->mNumVertices);

  for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices;
       vertexIndex++) {
    aiVector3D position = mesh->mVertices[vertexIndex];
    aiVector3D normal = mesh->mNormals[vertexIndex];
    aiVector3D texCoord = mesh->mTextureCoords[0][vertexIndex];

    vertices[vertexIndex].position =
        glm::vec3(position.x, position.y, position.z);
    vertices[vertexIndex].normal = glm::vec3(normal.x, normal.y, normal.z);
    // Take only the UV coordinates
    // Invert the Y coordinate into range [0, 1]
    vertices[vertexIndex].tex_coord = glm::vec2(texCoord.x, -texCoord.y);
  }

  // Three indices per triangle face
  index_count = mesh->mNumFaces * 3;
  std::vector<MeshIndex> indices(index_count);

  // lol
  uint32_t index_index = 0;
  for (uint32_t face_index = 0; face_index < mesh->mNumFaces; face_index++) {
    aiFace& face = mesh->mFaces[face_index];

    for (uint32_t corner_index = 0; corner_index < face.mNumIndices;
         corner_index++) {
      indices[index_index] = face.mIndices[corner_index];
      index_index++;
    }
  }

  size_t vertex_size = sizeof(MeshVertex) * vertices.size();
  vertex_buffer =
      new GpuBuffer(gpu, vertex_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VMA_MEMORY_USAGE_CPU_TO_GPU);
  vertex_buffer->writeData(vertices.data());

  size_t index_size = sizeof(indices[0]) * indices.size();
  index_buffer =
      new GpuBuffer(gpu, index_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                    VMA_MEMORY_USAGE_CPU_TO_GPU);
  index_buffer->writeData(indices.data());
}

MeshAsset::~MeshAsset() {
  log_dbg("Destroying mesh asset:");
  log_dbg(mesh_name.c_str());

  if (vertex_buffer != nullptr) delete vertex_buffer;
  if (index_buffer != nullptr) delete index_buffer;
}

}  // namespace mondradiko
