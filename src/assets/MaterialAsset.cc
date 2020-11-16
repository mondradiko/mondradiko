/**
 * @file MaterialAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains textures and settings for a mesh material.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
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
