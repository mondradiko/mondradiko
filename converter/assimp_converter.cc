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

#include "converter/assimp_converter.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/texture.h>

#include <assimp/Importer.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "assets/format/MaterialAsset.h"
#include "assets/format/MeshAsset.h"
#include "converter/stb_converter.h"
#include "log/log.h"

namespace mondradiko {

bool convert_assimp(assets::AssetBundleBuilder* builder,
                    std::filesystem::path scene_path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(scene_path.c_str(),
                                           aiProcessPreset_TargetRealtime_Fast);

  if (!scene) {
    log_ftl("Failed to load Assimp scene from %s", scene_path.c_str());
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
      vertex.color = glm::vec3(1.0, 1.0, 1.0);
      // Take only the UV coordinates
      // Invert the Y coordinate into range [0, 1]
      vertex.tex_coord = glm::vec2(texCoord.x, -texCoord.y);

      mesh_asset << vertex;
    }

    for (uint32_t face_index = 0; face_index < mesh->mNumFaces; face_index++) {
      aiFace& face = mesh->mFaces[face_index];

      for (uint32_t corner_index = 0; corner_index < face.mNumIndices;
           corner_index++) {
        assets::MeshIndex index = face.mIndices[corner_index];

        mesh_asset << index;
      }
    }

    assets::AssetId mesh_id;
    builder->addAsset(&mesh_id, &mesh_asset);
    log_inf("Created mesh asset with ID 0x%08x", mesh_id);
  }

  {
    // TODO(marceline-cramer) Replace Assimp
    std::filesystem::path albedo_path =
        scene_path.parent_path() / "Default_albedo.jpg";
    log_inf("Loading albedo texture from %s", albedo_path.c_str());

    assets::MutableAsset material_asset;

    assets::MaterialHeader header;
    header.albedo_factor = glm::vec4(1.0, 0.0, 1.0, 1.0);
    header.albedo_texture = stb_convert(builder, albedo_path);

    material_asset << header;

    assets::AssetId material_id;
    builder->addAsset(&material_id, &material_asset);
    log_inf("Created material asset with ID 0x%08x", material_id);
  }

  return true;
}

}  // namespace mondradiko
