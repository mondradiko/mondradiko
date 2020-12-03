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

#include "renderer/Renderer.h"

namespace mondradiko {

MaterialAsset::MaterialAsset(Renderer *renderer, std::string filename,
                             const aiScene *model_scene, aiMaterial *material)
    : renderer(renderer) {
  aiString baseColor;
  material->GetTexture(aiTextureType_DIFFUSE, 0, &baseColor);
  albedo_texture = renderer->loadTexture(filename, model_scene, baseColor);
}

MaterialAsset::~MaterialAsset() {}

void MaterialAsset::updateUniform(MaterialUniform *uniform) {
  uniform->albedo_texture = albedo_texture->index;
}

void MaterialAsset::updateDescriptor(GpuDescriptorSet *descriptor_set) {
  descriptor_set->updateImage(0, albedo_texture->image);
}

}  // namespace mondradiko
