/**
 * @file Asset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores the raw Asset data found in a lump.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/Asset.h"

namespace mondradiko {
namespace assets {

const char* getAssetResultString(AssetResult result) {
  switch (result) {
    case AssetResult::Success:
      return "Success";
    case AssetResult::FileNotFound:
      return "FileNotFound";
    case AssetResult::UnexpectedEof:
      return "UnexpectedEof";
    case AssetResult::BadFile:
      return "BadFile";
    case AssetResult::WrongMagic:
      return "WrongMagic";
    case AssetResult::WrongVersion:
      return "WrongVersion";
    case AssetResult::BadSize:
      return "BadSize";
    case AssetResult::InvalidChecksum:
      return "InvalidChecksum";
    case AssetResult::DuplicateAsset:
      return "DuplicateAsset";
    default:
      return "Unknown result";
  }
}

}  // namespace assets
}  // namespace mondradiko
