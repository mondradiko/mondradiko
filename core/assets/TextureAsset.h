// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"

namespace mondradiko {

// Forward declarations
class GpuImage;
class GpuInstance;

class TextureAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::TextureAsset);

  // Asset lifetime implementation
  explicit TextureAsset(GpuInstance* gpu) : gpu(gpu) {}
  void load(const assets::SerializedAsset*) final;
  ~TextureAsset();

  GpuImage* getImage() const { return image; }

 private:
  GpuInstance* gpu;

  GpuImage* image = nullptr;
};

}  // namespace mondradiko
