/**
 * @file AssetLump.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Stores a contiguous set of Assets in binary.
 * @date 2020-12-08
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "assets/loading/AssetLump.h"

#include <fstream>
#include <vector>

#include "log/log.h"
#include "lz4frame.h"  // NOLINT
#include "xxhash.h"    // NOLINT

namespace mondradiko {
namespace assets {

const uint32_t ASSET_LOAD_CHUNK_SIZE = 1024;
static_assert(ASSET_LOAD_CHUNK_SIZE >= LZ4F_HEADER_SIZE_MAX);

AssetLump::AssetLump(const std::filesystem::path& lump_path)
    : lump_path(lump_path) {
  log_dbg("Loading lump %s", lump_path.c_str());
}

AssetLump::~AssetLump() {
  log_dbg("Unloading lump %s", lump_path.c_str());

  if (loaded_data) delete[] loaded_data;
}

bool AssetLump::assertLength(size_t check_size) {
  log_dbg("Asserting size of lump %s", lump_path.c_str());

  size_t lump_length = std::filesystem::file_size(lump_path);

  if (lump_length != check_size) {
    log_err("Lump size assertion failed (expected 0x%08x bytes, got 0x%08x)",
            check_size, lump_length);
    return false;
  }

  return true;
}

bool AssetLump::assertHash(LumpHashMethod hash_method, LumpHash checksum) {
  LumpHash computed_hash;

  log_dbg("Asserting hash from lump %s", lump_path.c_str());

  std::ifstream lump_file(lump_path.c_str(), std::ifstream::binary);

  switch (hash_method) {
    case LumpHashMethod::xxHash: {
      log_inf("Hashing lump with xxHash");

      char buffer[ASSET_LOAD_CHUNK_SIZE];
      XXH3_state_t* hash_state = XXH3_createState();
      XXH3_64bits_reset(hash_state);

      while (!lump_file.eof()) {
        lump_file.read(buffer, sizeof(buffer));
        auto bytes_read = lump_file.gcount();
        if (bytes_read) XXH3_64bits_update(hash_state, buffer, bytes_read);
      }

      computed_hash = static_cast<LumpHash>(XXH3_64bits_digest(hash_state));
      XXH3_freeState(hash_state);
      break;
    }

    case LumpHashMethod::None: {
      log_dbg("Lump has no hash method; approving");
      lump_file.close();
      return true;
    }

    default: {
      log_err("Unrecognized lump hash method");
      lump_file.close();
      return false;
    }
  }  // switch (hash_method)

  lump_file.close();

  if (computed_hash == checksum) {
    log_inf("Checksum passed with value 0x%016lx", checksum);
    return true;
  } else {
    log_err("Calculated hash 0x%016lx does not match checksum 0x%016lx",
            computed_hash, checksum);
    return false;
  }
}

void AssetLump::decompress(LumpCompressionMethod compression_method) {
  if (loaded_data) return;

  std::ifstream lump_file(lump_path.c_str(), std::ifstream::binary);
  lump_file.seekg(0, std::ifstream::end);
  size_t file_size = lump_file.tellg();
  lump_file.seekg(0);

  switch (compression_method) {
    case LumpCompressionMethod::LZ4: {
      log_dbg("Decompressing lump %s with LZ4", lump_path.c_str());

      LZ4F_dctx* context;
      LZ4F_createDecompressionContext(&context, LZ4F_VERSION);

      std::vector<char> buffer(ASSET_LOAD_CHUNK_SIZE);

      {
        lump_file.read(buffer.data(), buffer.size());
        size_t bytes_read = lump_file.gcount();

        LZ4F_frameInfo_t frame_info;
        size_t result =
            LZ4F_getFrameInfo(context, &frame_info, buffer.data(), &bytes_read);

        if (LZ4F_isError(result)) {
          log_ftl("LZ4 decompression error");
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

    default: {
      log_wrn("Unrecognized lump compression method");
    }

    case LumpCompressionMethod::None: {
      log_dbg("Loading lump %s directly from disk", lump_path.c_str());

      loaded_size = file_size;
      loaded_data = new char[loaded_size];
      lump_file.read(loaded_data, loaded_size);
      break;
    }
  }

  lump_file.close();
}

bool AssetLump::loadAsset(const SerializedAsset** asset, size_t offset,
                          size_t size) {
  if (offset + size > loaded_size) {
    log_err("Asset range exceeds lump size of 0x%08x", loaded_size);
    return false;
  }

  const uint8_t* asset_data =
      reinterpret_cast<const uint8_t*>(loaded_data + offset);

  flatbuffers::Verifier verifier(asset_data, size);
  if (!VerifySerializedAssetBuffer(verifier)) {
    log_err("Asset validation failed");
    return false;
  }

  *asset = GetSerializedAsset(asset_data);

  return true;
}

}  // namespace assets
}  // namespace mondradiko
