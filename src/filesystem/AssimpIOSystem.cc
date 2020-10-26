/**
 * @file AssimpIOSystem.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits Assimp::IOSystem to integrate Assimp into the Filesystem.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include "filesystem/AssimpIOSystem.h"

#include "filesystem/AssimpIOStream.h"
#include "filesystem/Filesystem.h"
#include "log/log.h"

AssimpIOSystem::AssimpIOSystem(Filesystem* fs) : fs(fs) {}

AssimpIOSystem::~AssimpIOSystem() {}

bool AssimpIOSystem::Exists(const char* fileName) const {
  return fs->exists(fileName);
}

Assimp::IOStream* AssimpIOSystem::Open(const char* fileName, const char* mode) {
  return new AssimpIOStream(fs, fileName, mode);
}

void AssimpIOSystem::Close(Assimp::IOStream* file) {}
