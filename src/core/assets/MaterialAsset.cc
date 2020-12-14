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

namespace mondradiko {

MaterialAsset::MaterialAsset(assets::ImmutableAsset& asset, GpuInstance* gpu)
    : gpu(gpu) {
  assets::MaterialHeader header;
  asset >> header;

  uniform.albedo_factor = header.albedo_factor;
}

void MaterialAsset::updateTextureDescriptor(GpuDescriptorSet* descriptor) const {}

}  // namespace mondradiko
