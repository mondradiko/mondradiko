/**
 * @file MaterialAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains textures and settings for a mesh material.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/MaterialAsset.h"

#include <string>

#include "renderer/MeshPipeline.h"

namespace mondradiko {

MaterialAsset::MaterialAsset(MeshPipeline *mesh_pipeline, std::string filename,
                             const aiScene *model_scene, aiMaterial *material)
    : mesh_pipeline(mesh_pipeline) {
  aiString baseColor;
  material->GetTexture(aiTextureType_DIFFUSE, 0, &baseColor);
  albedo_texture = mesh_pipeline->loadTexture(filename, model_scene, baseColor);
}

MaterialAsset::~MaterialAsset() {}

void MaterialAsset::updateUniform(MaterialUniform *uniform) {
  uniform->albedo_texture = albedo_texture->index;
}

}  // namespace mondradiko
