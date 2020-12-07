/**
 * @file AssimpIOSystem.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits Assimp::IOSystem to integrate Assimp into the Filesystem.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "filesystem/AssimpIOSystem.h"

#include "filesystem/AssimpIOStream.h"
#include "filesystem/Filesystem.h"
#include "log/log.h"

namespace mondradiko {

AssimpIOSystem::AssimpIOSystem(Filesystem* fs) : fs(fs) {}

AssimpIOSystem::~AssimpIOSystem() {}

bool AssimpIOSystem::Exists(const char* fileName) const {
  return fs->exists(fileName);
}

Assimp::IOStream* AssimpIOSystem::Open(const char* fileName, const char* mode) {
  return new AssimpIOStream(fs, fileName, mode);
}

void AssimpIOSystem::Close(Assimp::IOStream* file) {}

}  // namespace mondradiko
