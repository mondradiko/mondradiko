/**
 * @file TextureAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Contains a texture, intended for mapping to meshes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>

#include "core/assets/Asset.h"
#include "core/gpu/GpuImage.h"
#include "core/gpu/GpuInstance.h"
#include "core/common/api_headers.h"

namespace mondradiko {

class TextureAsset : public Asset {
 public:
  TextureAsset(GpuInstance*, aiTexture*);
  ~TextureAsset();

  /**
   * @brief Stores the index into the CURRENT FRAME's descriptor array.
   *
   */
  uint32_t index = 0;

  GpuImage* image = nullptr;

 private:
  GpuInstance* gpu;
};

}  // namespace mondradiko
