/**
 * @file TextureAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/Asset.h"
#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations
class GpuImage;
class GpuInstance;

class TextureAsset : public Asset {
 public:
  TextureAsset(assets::ImmutableAsset&, GpuInstance*);
  ~TextureAsset();

  GpuImage* getImage() const { return image; }

 private:
  GpuInstance* gpu;

  GpuImage* image = nullptr;
};

}  // namespace mondradiko
