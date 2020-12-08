/**
 * @file AssimpIOSystem.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits Assimp::IOSystem to integrate Assimp into the Filesystem.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/filesystem/Filesystem.h"
#include "core/common/api_headers.h"

namespace mondradiko {

class AssimpIOSystem : public Assimp::IOSystem {
 public:
  explicit AssimpIOSystem(Filesystem*);
  ~AssimpIOSystem();

  bool Exists(const char*) const;
  char getOsSeparator() const { return '/'; }
  Assimp::IOStream* Open(const char*, const char*);
  void Close(Assimp::IOStream*);

 private:
  Filesystem* fs;
};

}  // namespace mondradiko
