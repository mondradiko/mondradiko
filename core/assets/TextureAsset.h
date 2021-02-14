// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "core/assets/AssetPool.h"

namespace mondradiko {

// Forward declarations
class GpuImage;
class MeshPass;

class TextureAsset : public Asset {
 public:
  DECL_ASSET_TYPE(assets::AssetType::TextureAsset);

  // Asset lifetime implementation
  explicit TextureAsset(MeshPass* mesh_pass) : mesh_pass(mesh_pass) {}
  void load(const assets::SerializedAsset*) final;
  ~TextureAsset();

  GpuImage* getImage() const { return image; }

 private:
  MeshPass* mesh_pass;

  GpuImage* image = nullptr;
};

}  // namespace mondradiko
