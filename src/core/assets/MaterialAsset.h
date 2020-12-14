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
#include "core/gpu/GpuDescriptorSet.h"

namespace mondradiko {

struct MaterialUniform {
  glm::vec4 albedo_factor;
  //glm::vec1 metallic_factor;
  //glm::vec1 roughness_factor;
};

class MaterialAsset : public Asset {
 public:
  MaterialAsset(assets::ImmutableAsset&, GpuInstance*);

  const MaterialUniform& getUniform() const { return uniform; };
  void updateTextureDescriptor(GpuDescriptorSet*) const;

 private:
  GpuInstance* gpu;

  MaterialUniform uniform;
};

}  // namespace mondradiko
