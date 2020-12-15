/**
 * @file Asset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Base class for a core asset and its ID.
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "assets/format/AssetTypes.h"
#include "assets/loading/ImmutableAsset.h"

namespace mondradiko {

using AssetId = assets::AssetId;

class Asset {
 private:
  AssetId id_;
  bool loaded;

  friend class AssetPool;
};

}  // namespace mondradiko
