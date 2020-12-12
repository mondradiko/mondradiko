/**
 * @file assimp_converter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-10
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "converter/MeshAsset/assimp_converter.h"

#include <iostream>
#include <sstream>
#include <vector>

#include "assets/format/MeshAsset.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/IOStream.hpp"
#include "assimp/IOSystem.hpp"
#include "assimp/Importer.hpp"
#include "assimp/Logger.hpp"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/texture.h"

namespace mondradiko {

bool convert_assimp(assets::AssetBundleBuilder* builder,
                    const char* asset_name) {
  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(asset_name, aiProcessPreset_TargetRealtime_Fast);

  if (!scene) {
    std::cerr << "Failed to read scene." << std::endl;
    return false;
  }

  for (uint32_t mesh_index = 0; mesh_index < scene->mNumMeshes; mesh_index++) {
    const auto& mesh = scene->mMeshes[mesh_index];

    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) !=
        aiPrimitiveType_TRIANGLE) {
      std::cerr << "Mesh topology is not exclusively triangles." << std::endl;
      return false;
    }

    if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) !=
        aiPrimitiveType_TRIANGLE) {
      std::cerr << "Mesh topology is not exclusively triangles." << std::endl;
      return false;
    }

    if (!mesh->HasPositions()) {
      std::cerr << "Mesh vertices have no positions." << std::endl;
      return false;
    }

    if (!mesh->HasNormals()) {
      std::cerr << "Mesh vertices have no normals." << std::endl;
      return false;
    }

    if (!mesh->HasTextureCoords(0)) {
      std::cerr << "Mesh vertices have no texture coordinates." << std::endl;
      return false;
    }

    assets::MutableAsset mesh_asset;

    assets::MeshHeader header;
    header.vertex_count = mesh->mNumVertices;
    // Three indices per triangle face
    header.index_count = mesh->mNumFaces * 3;

    mesh_asset << header;

    for (uint32_t vertex_index = 0; vertex_index < mesh->mNumVertices;
         vertex_index++) {
      aiVector3D position = mesh->mVertices[vertex_index];
      aiVector3D normal = mesh->mNormals[vertex_index];
      aiVector3D texCoord = mesh->mTextureCoords[0][vertex_index];

      assets::MeshVertex vertex;
      vertex.position = glm::vec3(position.x, position.y, position.z);
      vertex.normal = glm::vec3(normal.x, normal.y, normal.z);
      // Take only the UV coordinates
      // Invert the Y coordinate into range [0, 1]
      vertex.tex_coord = glm::vec2(texCoord.x, -texCoord.y);

      mesh_asset << vertex;
    }

    // lol
    uint32_t index_index = 0;
    for (uint32_t face_index = 0; face_index < mesh->mNumFaces; face_index++) {
      aiFace& face = mesh->mFaces[face_index];

      for (uint32_t corner_index = 0; corner_index < face.mNumIndices;
           corner_index++) {
        assets::MeshIndex index = face.mIndices[corner_index];

        mesh_asset << index;
      }
    }

    builder->addAsset(0xdeadbeef, &mesh_asset);
  }

  return true;
}

}  // namespace mondradiko
