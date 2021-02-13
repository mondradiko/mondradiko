// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/MaterialAsset.h"

#include "core/assets/Asset.h"
#include "core/assets/TextureAsset.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuInstance.h"
#include "types/assets/MaterialAsset_generated.h"

namespace mondradiko {

void MaterialAsset::load(const assets::SerializedAsset* asset) {
  const assets::MaterialAsset* material = asset->material();

  albedo_texture = asset_pool->load<TextureAsset>(material->albedo_texture());

  if (material->emissive_texture() != AssetId::NullAsset) {
    emissive_texture =
        asset_pool->load<TextureAsset>(material->emissive_texture());
    uniform.has_emissive_texture = 1;
  } else {
    uniform.has_emissive_texture = 0;
  }

  if (material->metal_roughness_texture() != AssetId::NullAsset) {
    metal_roughness_texture =
        asset_pool->load<TextureAsset>(material->metal_roughness_texture());
    uniform.has_metal_roughness_texture = 1;
  } else {
    uniform.has_metal_roughness_texture = 0;
  }

  const assets::Vec3* emissive_factor = material->emissive_factor();
  uniform.emissive_factor = glm::vec3(
      emissive_factor->x(), emissive_factor->y(), emissive_factor->z());

  const assets::Vec4* albedo_factor = material->albedo_factor();
  uniform.albedo_factor = glm::vec4(albedo_factor->x(), albedo_factor->y(),
                                    albedo_factor->z(), albedo_factor->w());

  uniform.mask_threshold = material->mask_threshold();
  uniform.metallic_factor = material->metallic_factor();
  uniform.roughness_factor = material->roughness_factor();
}

void MaterialAsset::updateTextureDescriptor(
    GpuDescriptorSet* descriptor) const {
  descriptor->updateImage(0, albedo_texture->getImage());

  if (uniform.has_emissive_texture) {
    descriptor->updateImage(1, emissive_texture->getImage());
  } else {
    // TODO(marceline-cramer) Standard dummy textures for missing slots
    descriptor->updateImage(1, albedo_texture->getImage());
  }

  if (uniform.has_metal_roughness_texture) {
    descriptor->updateImage(2, metal_roughness_texture->getImage());
  } else {
    // TODO(marceline-cramer) Standard dummy textures for missing slots
    descriptor->updateImage(2, albedo_texture->getImage());
  }
}

}  // namespace mondradiko
