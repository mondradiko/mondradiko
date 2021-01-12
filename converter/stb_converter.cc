/**
 * @file stb_converter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates a TextureAsset from an STB-loaded image.
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "converter/stb_converter.h"

#include <ktx.h>
#include <vulkan/vulkan.h>

#include "assets/format/TextureAsset_generated.h"
#include "lib/stb_image.h"
#include "log/log.h"

namespace mondradiko {

assets::AssetId stb_convert(assets::AssetBundleBuilder* builder,
                            std::filesystem::path texture_path) {
  if (!std::filesystem::exists(texture_path)) {
    log_ftl("Texture %s does not exist", texture_path.c_str());
  }

  int channels = 4;

  int width, height, file_channels;
  unsigned char* texture_src = stbi_load(texture_path.c_str(), &width, &height,
                                         &file_channels, channels);

  ktxTexture2* texture;
  ktxTextureCreateInfo create_info;
  KTX_error_code result;

  create_info.vkFormat = VK_FORMAT_R8G8B8A8_UNORM;

  create_info.baseWidth = width;
  create_info.baseHeight = height;

  create_info.baseDepth = 1;
  create_info.numDimensions = 2;
  create_info.numLevels = 1;
  create_info.numLayers = 1;
  create_info.numFaces = 1;
  create_info.isArray = KTX_FALSE;
  create_info.generateMipmaps = KTX_TRUE;

  result = ktxTexture2_Create(&create_info, KTX_TEXTURE_CREATE_ALLOC_STORAGE,
                              &texture);

  if (result != KTX_SUCCESS) {
    log_ftl("KTX texture creation failed with error code %d", result);
  }

  size_t src_size = width * height * channels;

  ktxTexture_SetImageFromMemory(ktxTexture(texture), 0, 0, 0, texture_src,
                                src_size);

  ktx_uint8_t* texture_data;
  ktx_size_t texture_size;
  ktxTexture_WriteToMemory(ktxTexture(texture), &texture_data, &texture_size);

  flatbuffers::FlatBufferBuilder fbb;

  auto data_offset = fbb.CreateVector(texture_data, texture_size);

  free(texture_data);
  ktxTexture_Destroy(ktxTexture(texture));
  stbi_image_free(texture_src);

  assets::TextureAssetBuilder texture_asset(fbb);
  texture_asset.add_format(assets::TextureFormat::KTX);
  texture_asset.add_data(data_offset);
  auto texture_offset = texture_asset.Finish();

  assets::SerializedAssetBuilder asset(fbb);
  asset.add_type(assets::AssetType::TextureAsset);
  asset.add_texture(texture_offset);
  auto asset_offset = asset.Finish();

  assets::AssetId texture_id;
  builder->addAsset(&texture_id, &fbb, asset_offset);
  return texture_id;
}

}  // namespace mondradiko
