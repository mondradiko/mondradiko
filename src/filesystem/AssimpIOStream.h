/**
 * @file AssimpIOStream.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits the Assimp::IOStream class to provide logging functionality.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_FILESYSTEM_ASSIMPIOSTREAM_H_
#define SRC_FILESYSTEM_ASSIMPIOSTREAM_H_

#include <vector>

#include "filesystem/Filesystem.h"
#include "src/api_headers.h"

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

  std::vector<char> contents;
  size_t position;
};

#endif  // SRC_FILESYSTEM_ASSIMPIOSTREAM_H_
