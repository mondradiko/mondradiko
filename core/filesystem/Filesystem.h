/**
 * @file Filesystem.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to the read-only files in an asset archive.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <vector>

#include "core/common/api_headers.h"

namespace mondradiko {

class Filesystem {
 public:
  explicit Filesystem(const char*);
  ~Filesystem();

  const char* archive;

  bool exists(const char*);
  bool loadBinaryFile(const char*, std::vector<unsigned char>*);

 private:
};

}  // namespace mondradiko
