/**
 * @file MaterialAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/MaterialAsset.h"

#include "assets/format/MaterialAsset.h"
#include "core/assets/Asset.h"
#include "core/assets/TextureAsset.h"
#include "core/gpu/GpuDescriptorSet.h"

namespace mondradiko {

MaterialAsset::MaterialAsset(assets::ImmutableAsset& asset,
                             AssetPool* asset_pool, GpuInstance* gpu)
    : asset_pool(asset_pool), gpu(gpu) {
  assets::MaterialHeader header;
  asset >> header;

  albedo_texture = asset_pool->loadAsset<TextureAsset>(header.albedo_texture, gpu);

  uniform.albedo_factor = header.albedo_factor;
}

void MaterialAsset::updateTextureDescriptor(
    GpuDescriptorSet* descriptor) const {
  descriptor->updateImage(
      0, asset_pool->getAsset<TextureAsset>(albedo_texture)->getImage());
}

}  // namespace mondradiko
