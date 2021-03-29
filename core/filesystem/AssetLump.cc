// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/filesystem/AssetLump.h"

#include <fstream>

#include "log/log.h"
#include "lz4frame.h"  // NOLINT
#include "types/containers/vector.h"
#include "xxhash.h"  // NOLINT

namespace mondradiko {
namespace core {

// using is ok here because it'd be inconvenient not to use it
using namespace assets;  // NOLINT

const uint32_t ASSET_LOAD_CHUNK_SIZE = 4 * 1024;  // 4 KiB
static_assert(ASSET_LOAD_CHUNK_SIZE >= LZ4F_HEADER_SIZE_MAX);

AssetLump::AssetLump(const std::filesystem::path& lump_path)
    : lump_path(lump_path) {
  log_zone;
  log_dbg_fmt("Loading lump %s", lump_path.c_str());
}

AssetLump::~AssetLump() {
  log_zone;
  log_dbg_fmt("Unloading lump %s", lump_path.c_str());

  if (loaded_data) delete[] loaded_data;
}

bool AssetLump::assertFileSize(size_t check_size) {
  log_zone;
  log_inf_fmt("Asserting size of lump file %s", lump_path.c_str());

  size_t lump_length = std::filesystem::file_size(lump_path);

  if (lump_length != check_size) {
    log_err_fmt(
        "Lump size assertion failed (expected 0x%08x bytes, got 0x%08x)",
        check_size, lump_length);
    return false;
  }

  return true;
}

bool AssetLump::assertHash(LumpHashMethod hash_method, LumpHash checksum) {
  log_zone;
  log_inf_fmt("Asserting hash from lump %s", lump_path.c_str());

  LumpHash computed_hash;

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
      log_inf("Lump has no hash method; approving");
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
    log_inf_fmt("Checksum passed with value 0x%016lx", checksum);
    return true;
  } else {
    log_err_fmt("Calculated hash 0x%016lx does not match checksum 0x%016lx",
                computed_hash, checksum);
    return false;
  }
}

void AssetLump::decompress(LumpCompressionMethod compression_method) {
  if (loaded_data) return;
  log_zone;

  std::ifstream lump_file(lump_path.c_str(), std::ifstream::binary);
  lump_file.seekg(0, std::ifstream::end);
  size_t file_size = lump_file.tellg();
  lump_file.seekg(0);

  switch (compression_method) {
    case LumpCompressionMethod::LZ4: {
      log_inf_fmt("Decompressing lump %s with LZ4", lump_path.c_str());

      LZ4F_dctx* context;
      LZ4F_createDecompressionContext(&context, LZ4F_VERSION);

      types::vector<char> buffer(ASSET_LOAD_CHUNK_SIZE);

      {
        lump_file.read(buffer.data(), buffer.size());
        size_t bytes_read = lump_file.gcount();

        LZ4F_frameInfo_t frame_info;
        size_t result =
            LZ4F_getFrameInfo(context, &frame_info, buffer.data(), &bytes_read);

        if (LZ4F_isError(result)) {
          log_ftl_fmt("Failed to read LZ4 frame info: %s",
                      LZ4F_getErrorName(result));
        }

        loaded_size = frame_info.contentSize;

        if (loaded_size == 0) {
          log_ftl("LZ4 compressed lump must contain content size");
        }

        loaded_data = new char[loaded_size];
        buffer.resize(result);
        lump_file.seekg(bytes_read);
      }

      int remaining_size = loaded_size;
      char* decompressed_data = loaded_data;

      while (!lump_file.eof()) {
        lump_file.read(buffer.data(), buffer.size());
        size_t bytes_read = lump_file.gcount();

        size_t decompressed_size = remaining_size;
        size_t bytes_consumed = bytes_read;
        size_t buf_hint =
            LZ4F_decompress(context, decompressed_data, &decompressed_size,
                            buffer.data(), &bytes_consumed, nullptr);

        if (LZ4F_isError(buf_hint)) {
          log_err_fmt("LZ4 decompression failed: %s",
                      LZ4F_getErrorName(buf_hint));
          break;
        }

        decompressed_data += decompressed_size;
        remaining_size -= decompressed_size;

        if (remaining_size < 0) {
          log_err("LZ4 decompression overflow");
          break;
        }

        if (bytes_consumed < bytes_read) {
          log_err("LZ4 decompression underflow");
        }

        if (buf_hint <= ASSET_LOAD_CHUNK_SIZE) {
          buffer.resize(buf_hint);
        }
      }

      LZ4F_freeDecompressionContext(context);
      break;
    }

    default: {
      log_wrn("Unrecognized lump compression method");
    }

    case LumpCompressionMethod::None: {
      log_dbg_fmt("Loading lump %s directly from disk", lump_path.c_str());

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
    log_err_fmt("Asset range exceeds lump size of 0x%0lx", loaded_size);
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

}  // namespace core
}  // namespace mondradiko
