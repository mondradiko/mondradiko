/**
 * @file AssimpIOSystem.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits Assimp::IOSystem to integrate Assimp into the Filesystem.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_FILESYSTEM_ASSIMPIOSYSTEM_H_
#define SRC_FILESYSTEM_ASSIMPIOSYSTEM_H_

#include "filesystem/Filesystem.h"
#include "src/api_headers.h"

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

#endif  // SRC_FILESYSTEM_ASSIMPIOSYSTEM_H_
