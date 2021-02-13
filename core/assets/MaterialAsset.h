// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/Asset.h"
#include "core/assets/AssetPool.h"
#include "core/assets/TextureAsset.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {

// Forward declarations
class GpuDescriptorSet;
class GpuInstance;

struct MaterialUniform {
  glm::vec3 emissive_factor;
  uint32_t has_emissive_texture;

  glm::vec4 albedo_factor;
  float mask_threshold;

  float metallic_factor;
  float roughness_factor;

  uint32_t has_metal_roughness_texture;
};

class MaterialAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::MaterialAsset);

  // Asset lifetime implementation
  MaterialAsset(AssetPool* asset_pool, GpuInstance* gpu)
      : asset_pool(asset_pool), gpu(gpu) {}
  void load(const assets::SerializedAsset*) final;

  // Override isLoaded() to include the textures
  bool isLoaded() const final { return albedo_texture.isLoaded(); }

  const MaterialUniform& getUniform() const { return uniform; }
  void updateTextureDescriptor(GpuDescriptorSet*) const;

 private:
  AssetPool* asset_pool;
  GpuInstance* gpu;

  AssetHandle<TextureAsset> albedo_texture;
  AssetHandle<TextureAsset> emissive_texture;
  AssetHandle<TextureAsset> metal_roughness_texture;

  MaterialUniform uniform;
};

}  // namespace mondradiko
