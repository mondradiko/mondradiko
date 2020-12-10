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

#include "lz4frame.h"
#include "xxhash.h"

namespace mondradiko {
namespace assets {

const uint32_t ASSET_LOAD_CHUNK_SIZE = 1024;
static_assert(ASSET_LOAD_CHUNK_SIZE >= LZ4F_HEADER_SIZE_MAX);

std::string generateLumpName(uint32_t lump_index) {
  char buf[32];
  snprintf(buf, sizeof(buf), "lump_%04u.bin", lump_index);
  return buf;
}

AssetLump::AssetLump(const std::filesystem::path& lump_path)
    : lump_path(lump_path) {}

AssetLump::~AssetLump() {
  if (loaded_data) delete[] loaded_data;
}

bool AssetLump::assertLength(size_t check_size) {
  return std::filesystem::file_size(lump_path) == check_size;
}

bool AssetLump::assertHash(LumpHashMethod hash_method, LumpHash checksum) {
  LumpHash computed_hash;
  std::ifstream lump_file(lump_path.c_str(),
                          std::ifstream::in | std::ifstream::binary);

  switch (hash_method) {
    case LumpHashMethod::xxHash: {
      char buffer[ASSET_LOAD_CHUNK_SIZE];
      XXH32_state_t* hash_state = XXH32_createState();
      XXH32_reset(hash_state, 0);

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

void AssetLump::decompress(LumpCompressionMethod compression_method) {
  if (loaded_data) return;

  std::ifstream lump_file(lump_path.c_str(),
                          std::ifstream::in | std::ifstream::binary);
  lump_file.seekg(0, std::ifstream::end);
  size_t file_size = lump_file.tellg();
  lump_file.seekg(0);

  switch (compression_method) {
    case LumpCompressionMethod::LZ4: {
      LZ4F_dctx* context;
      LZ4F_createDecompressionContext(&context, LZ4F_VERSION);

      std::vector<char> buffer(ASSET_LOAD_CHUNK_SIZE);

      {
        lump_file.read(buffer.data(), buffer.size());
        size_t bytes_read = lump_file.gcount();

        LZ4F_frameInfo_t frame_info;
        size_t result = LZ4F_getFrameInfo(context, &frame_info, buffer.data(), &bytes_read);

        if (LZ4F_isError(result)) {
          std::cerr << "shite" << std::endl;
        }

        loaded_size = frame_info.contentSize;
        loaded_data = new char[loaded_size];

        buffer.resize(result);
        lump_file.seekg(bytes_read);
      }

      // TODO(marceline-cramer) Finish this

      /*while (!lump_file.eof()) {
        lump_file.read(buffer.data(), buffer.size());
        auto bytes_read = lump_file.gcount();
        LZ4F_decompress(context, )
      }*/

      LZ4F_freeDecompressionContext(context);
      break;
    }

    default:
    case LumpCompressionMethod::None: {
      loaded_size = file_size;
      loaded_data = new char[loaded_size];
      lump_file.read(loaded_data, loaded_size);
      break;
    }
  }

  std::cout << loaded_size << std::endl;
  // std::cout << loaded_data << std::endl;

  lump_file.close();
}

}  // namespace assets
}  // namespace mondradiko
