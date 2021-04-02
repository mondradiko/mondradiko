// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "types/assets/AssetTypes.h"
#include "types/assets/SerializedAsset_generated.h"

#define DECL_ASSET_TYPE(type) \
  static constexpr assets::AssetType ASSET_TYPE = type;

namespace mondradiko {
namespace core {

using AssetId = assets::AssetId;

enum class AssetState {
  Unloaded,    // Unusable, and not in memory
  Retrieving,  // Currently being decompressed and/or downloaded
  Stored,      // In memory, ready to load
  Loading,     // Being loaded
  Loaded,      // Ready to use
  Cached,      // Not currently being used; first candidate for unloading
  Unloading    // Being removed from memory
};

class Asset {
 public:
  virtual ~Asset() {}

  bool load(const assets::SerializedAsset*);
  virtual bool isLoaded() const { return _state == AssetState::Loaded; }

 protected:
  template <class AssetType>
  friend class AssetHandle;

  friend class AssetPool;

  virtual bool _load(const assets::SerializedAsset*) = 0;
  void _ref();
  void _unref();

 private:
  AssetState _state = AssetState::Unloaded;
  uint32_t ref_count = 0;
};

}  // namespace core
}  // namespace mondradiko
