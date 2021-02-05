// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/MaterialAsset.h"

#include "types/assets/MaterialAsset_generated.h"
#include "core/assets/Asset.h"
#include "core/assets/TextureAsset.h"
#include "core/gpu/GpuDescriptorSet.h"
#include "core/gpu/GpuInstance.h"

namespace mondradiko {

void MaterialAsset::load(const assets::SerializedAsset* asset) {
  const assets::MaterialAsset* material = asset->material();

  albedo_texture = asset_pool->load<TextureAsset>(material->albedo_texture());

  const assets::Vec3* albedo_factor = material->albedo_factor();
  uniform.albedo_factor = glm::vec4(albedo_factor->x(), albedo_factor->y(),
                                    albedo_factor->z(), 1.0);
}

void MaterialAsset::updateTextureDescriptor(
    GpuDescriptorSet* descriptor) const {
  descriptor->updateImage(0, albedo_texture->getImage());
}

}  // namespace mondradiko
