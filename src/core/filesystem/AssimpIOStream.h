/**
 * @file AssimpIOStream.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits the Assimp::IOStream class to provide logging functionality.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "filesystem/Filesystem.h"
#include "common/api_headers.h"

namespace mondradiko {

class AssimpIOStream : public Assimp::IOStream {
 public:
  AssimpIOStream(Filesystem*, const char*, const char*);
  ~AssimpIOStream();

  size_t Read(void*, size_t, size_t);
  size_t Write(const void*, size_t, size_t);
  aiReturn Seek(size_t, aiOrigin);
  size_t Tell() const;
  size_t FileSize() const;
  void Flush();

 private:
  Filesystem* fs;

  std::vector<unsigned char> contents;
  size_t position;
};

}  // namespace mondradiko
