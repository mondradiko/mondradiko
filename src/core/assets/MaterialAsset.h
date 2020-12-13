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
  uint32_t albedo_texture;
};

class MaterialAsset : public Asset {
 public:
  MaterialAsset(assets::ImmutableAsset&, GpuInstance*) {}

  void updateUniform(MaterialUniform*) const {}
  void updateDescriptor(GpuDescriptorSet*) const {}

 private:
};

}  // namespace mondradiko
