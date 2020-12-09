/**
 * @file AssetLump.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores a contiguous set of Assets in binary.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/AssetLump.h"

namespace mondradiko {
namespace assets {

AssetLump::AssetLump(const std::filesystem::path& lump_path)
    : lump_path(lump_path) {}

// TODO(marceline-cramer) Implement these
bool AssetLump::assertLength(size_t) { return false; }
bool AssetLump::assertHash(LumpHashMethod, LumpHash) { return false; }
void AssetLump::decompress(LumpCompressionMethod) {}

}  // namespace assets
}  // namespace mondradiko
