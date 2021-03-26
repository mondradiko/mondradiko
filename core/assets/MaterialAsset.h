// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/Asset.h"
#include "core/assets/AssetPool.h"
#include "core/assets/TextureAsset.h"
#include "lib/include/glm_headers.h"

namespace mondradiko {
namespace core {

// Forward declarations
class GpuDescriptorSet;
class GpuInstance;

struct MaterialUniform {
  glm::vec4 emissive_factor;
  glm::vec4 albedo_factor;

  float mask_threshold;
  float metallic_factor;
  float roughness_factor;
  float normal_map_scale;

  uint32_t is_unlit;
  uint32_t enable_blend;
  uint32_t has_emissive_texture;
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
  bool isDoubleSided() const { return double_sided; }
  void updateTextureDescriptor(GpuDescriptorSet*) const;

 private:
  AssetPool* asset_pool;
  GpuInstance* gpu;

  bool double_sided;

  AssetHandle<TextureAsset> albedo_texture;
  AssetHandle<TextureAsset> emissive_texture;
  AssetHandle<TextureAsset> normal_map_texture;
  AssetHandle<TextureAsset> metal_roughness_texture;

  MaterialUniform uniform;
};

}  // namespace core
}  // namespace mondradiko
