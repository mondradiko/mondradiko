/**
 * @file TextureAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-17
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/AssetPool.h"
#include "core/common/api_headers.h"

namespace mondradiko {

// Forward declarations
class GpuImage;
class GpuInstance;

class TextureAsset : public Asset {
 public:
  TextureAsset(AssetPool*, GpuInstance*);

  // AssetPool implementation
  void load(assets::ImmutableAsset&);
  void unload();

  GpuImage* getImage() const { return image; }

 private:
  GpuInstance* gpu;

  GpuImage* image = nullptr;
};

}  // namespace mondradiko
