// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "assets/common/AssetTypes.h"
#include "types/assets/SerializedAsset_generated.h"

#define DECL_ASSET_TYPE(type) \
  static constexpr assets::AssetType ASSET_TYPE = type;

namespace mondradiko {

using AssetId = assets::AssetId;

class Asset {
 public:
  virtual ~Asset() {}

  virtual void load(const assets::SerializedAsset*) = 0;

  virtual bool isLoaded() const { return loaded; }

 private:
  bool loaded = false;
  uint32_t ref_count = 0;

  template <class AssetType>
  friend class AssetHandle;

  friend class AssetPool;
};

}  // namespace mondradiko
