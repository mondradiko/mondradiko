/**
 * @file MaterialAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/Asset.h"
#include "core/assets/AssetPool.h"
#include "core/common/glm_headers.h"

namespace mondradiko {

// Forward declarations
class GpuDescriptorSet;
class GpuInstance;

struct MaterialUniform {
  glm::vec4 albedo_factor;
  // glm::vec1 metallic_factor;
  // glm::vec1 roughness_factor;
};

class MaterialAsset : public Asset {
 public:
  MaterialAsset(assets::ImmutableAsset&, AssetPool*, GpuInstance*);

  const MaterialUniform& getUniform() const { return uniform; }
  void updateTextureDescriptor(GpuDescriptorSet*) const;

 private:
  AssetPool* asset_pool;
  GpuInstance* gpu;

  AssetId albedo_texture;

  MaterialUniform uniform;
};

}  // namespace mondradiko
