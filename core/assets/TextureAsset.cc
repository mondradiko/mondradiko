/**
 * @file TextureAsset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/assets/TextureAsset.h"

#include "assets/format/TextureAsset.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "log/log.h"

namespace mondradiko {

TextureAsset::TextureAsset(assets::ImmutableAsset& asset, GpuInstance* gpu)
    : gpu(gpu) {
  assets::TextureHeader header;
  asset >> header;

  size_t texture_size;
  const char* texture_data = asset.getData(&texture_size);

  switch (header.format) {
    case assets::TextureFormat::KTX: {
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
          reinterpret_cast<const ktx_uint8_t*>(texture_data), texture_size,
          KTX_TEXTURE_CREATE_NO_FLAGS, &k_texture);

      if (k_result != KTX_SUCCESS) {
        log_ftl("Failed to create KTX texture");
      }

      image = new GpuImage(gpu, k_texture, VK_IMAGE_USAGE_SAMPLED_BIT);
      image->createView();

      ktxTexture_Destroy(k_texture);

      break;
    }

    default: {
      log_err("Unrecognized texture format");
      break;
    }
  }  // switch (header.format)
}

TextureAsset::~TextureAsset() {
  if (image != nullptr) delete image;
}

}  // namespace mondradiko
