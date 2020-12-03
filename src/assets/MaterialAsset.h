/**
 * @file MaterialAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains textures and settings for a mesh material.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_ASSETS_MATERIALASSET_H_
#define SRC_ASSETS_MATERIALASSET_H_

#include <string>

#include "assets/Asset.h"
#include "assets/AssetHandle.h"
#include "assets/TextureAsset.h"
#include "gpu/GpuDescriptorSet.h"
#include "src/api_headers.h"

namespace mondradiko {

class Renderer;  // Forward declaration because of codependence

struct MaterialUniform {
  uint32_t albedo_texture;
};

class MaterialAsset : public Asset {
 public:
  MaterialAsset(Renderer*, std::string, const aiScene*, aiMaterial*);
  ~MaterialAsset();

  void updateUniform(MaterialUniform*);
  void updateDescriptor(GpuDescriptorSet*);

  AssetHandle<TextureAsset> albedo_texture;

 private:
  Renderer* renderer;
};

}  // namespace mondradiko

#endif  // SRC_ASSETS_MATERIALASSET_H_
