// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/assets/Asset.h"

#include "log/log.h"

namespace mondradiko {
namespace core {

bool Asset::load(const assets::SerializedAsset* asset) {
  log_zone;

  _state = AssetState::Loading;

  bool load_result = _load(asset);
  if (load_result) {
    _state = AssetState::Loaded;
    return true;
  } else {
    _state = AssetState::Stored;
    return false;
  }
}

void Asset::_ref() {
  log_zone;

  ref_count++;
}

void Asset::_unref() {
  log_zone;

  ref_count--;

  if (_state == AssetState::Loaded) {
    if (ref_count <= 0) {
      _state = AssetState::Cached;
    }
  }
}
}  // namespace core
}  // namespace mondradiko
