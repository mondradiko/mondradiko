// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/TextureAsset.h"

#include "assets/format/TextureAsset_generated.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

void TextureAsset::load(const assets::SerializedAsset* asset) {
  const assets::TextureAsset* texture = asset->texture();

  VkFormat texture_format;

  // TODO(marceline-cramer) Support all texture types

  if (texture->components() != 4) {
    log_ftl("Unsupported number of components");
  }

  if (texture->component_type() != assets::TextureComponentType::UByte) {
    log_ftl("Unsupported component type");
  }

  if (texture->srgb()) {
    texture_format = VK_FORMAT_R8G8B8A8_SRGB;
  } else {
    texture_format = VK_FORMAT_R8G8B8A8_UNORM;
  }

  image =
      new GpuImage(gpu, texture_format, texture->width(), texture->height(),
                   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                   VMA_MEMORY_USAGE_GPU_ONLY);

  image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  image->writeData(texture->data()->data());
  image->transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

TextureAsset::~TextureAsset() {
  if (image) delete image;
}

}  // namespace mondradiko
