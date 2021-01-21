/**
 * @file TextureAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/TextureAsset.h"

#include "assets/format/TextureAsset_generated.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

void TextureAsset::load(const assets::SerializedAsset* asset) {
  const assets::TextureAsset* texture = asset->texture();

  switch (texture->format()) {
    case assets::TextureFormat::KTX: {
      const assets::KtxTexture* ktx_texture = texture->ktx();

      ktxTexture* k_texture;
      KTX_error_code k_result;
      ktxVulkanDeviceInfo vdi;

      k_result = ktxVulkanDeviceInfo_Construct(&vdi, gpu->physical_device,
                                               gpu->device, gpu->graphics_queue,
                                               gpu->command_pool, nullptr);

      if (k_result != KTX_SUCCESS) {
        log_ftl("Failed to set up libktx Vulkan device info");
      }

      k_result = ktxTexture_CreateFromMemory(
          reinterpret_cast<const ktx_uint8_t*>(ktx_texture->data()->data()),
          ktx_texture->data()->size(), KTX_TEXTURE_CREATE_NO_FLAGS, &k_texture);

      if (k_result != KTX_SUCCESS) {
        log_ftl("Failed to create KTX texture");
      }

      image = new GpuImage(gpu, k_texture, VK_IMAGE_USAGE_SAMPLED_BIT);

      ktxTexture_Destroy(k_texture);
      break;
    }

    case assets::TextureFormat::Raw: {
      const assets::RawTexture* raw_texture = texture->raw();

      VkFormat texture_format;

      // TODO(marceline-cramer) Support all texture types

      if (raw_texture->components() != 4) {
        log_ftl("Unsupported number of components");
      }

      if (raw_texture->component_type() !=
          assets::TextureComponentType::UByte) {
        log_ftl("Unsupported component type");
      }

      texture_format = VK_FORMAT_R8G8B8A8_UNORM;
      log_inf("%ux%u", raw_texture->width(), raw_texture->height());

      image = new GpuImage(
          gpu, texture_format, raw_texture->width(), raw_texture->height(),
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
          VMA_MEMORY_USAGE_GPU_ONLY);

      image->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
      image->writeData(raw_texture->data()->data());
      image->transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
      break;
    }

    default: {
      log_ftl("Unrecognized texture format");
    }
  }  // switch (header.format)
}

TextureAsset::~TextureAsset() {
  if (image) delete image;
}

}  // namespace mondradiko
