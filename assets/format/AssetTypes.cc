/**
 * @file Asset.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements AssetType.h's helper functions.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/format/AssetTypes.h"

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

std::string generateLumpName(uint32_t lump_index) {
  char buf[32];
  snprintf(buf, sizeof(buf), "lump_%04u.bin", lump_index);
  return buf;
}

}  // namespace assets
}  // namespace mondradiko
