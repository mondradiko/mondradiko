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

#include "xxhash.h"

namespace mondradiko {
namespace assets {

AssetLump::AssetLump(const std::filesystem::path& lump_path)
    : lump_path(lump_path) {}

// TODO(marceline-cramer) Implement these
bool AssetLump::assertLength(size_t) { return false; }

bool AssetLump::assertHash(LumpHashMethod hash_method, LumpHash checksum) {
  LumpHash computed_hash;
  std::ifstream lump_file(lump_path.c_str(),
                          std::ifstream::in | std::ifstream::binary);

  switch (hash_method) {
    case LumpHashMethod::xxHash: {
      char buffer[256];
      XXH32_state_t* hash_state = XXH32_createState();

      while (!lump_file.eof()) {
        lump_file.read(buffer, sizeof(buffer));
        auto bytes_read = lump_file.gcount();
        if (bytes_read) XXH32_update(hash_state, buffer, bytes_read);
      }

      computed_hash = XXH32_digest(hash_state);
      XXH32_freeState(hash_state);
      break;
    }

    default:
    case LumpHashMethod::None: {
      lump_file.close();
      return false;
    }
  }  // switch (hash_method)

  lump_file.close();
  return computed_hash == checksum;
}

void AssetLump::decompress(LumpCompressionMethod) {}

}  // namespace assets
}  // namespace mondradiko
