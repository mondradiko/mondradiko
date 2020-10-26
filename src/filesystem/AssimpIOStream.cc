/**
 * @file AssimpIOStream.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Inherits the Assimp::IOStream class to provide logging functionality.
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

#include "filesystem/AssimpIOStream.h"

#include "filesystem/Filesystem.h"

AssimpIOStream::AssimpIOStream(Filesystem* fs, const char* fileName,
                               const char* mode)
    : fs(fs) {
  fs->loadBinaryFile(fileName, &contents);
  position = 0;
}

AssimpIOStream::~AssimpIOStream() {}

size_t AssimpIOStream::Read(void* data, size_t size, size_t count) {
  size_t readNum = size * count;
  // TODO(marceline-cramer) This isn't safe
  memcpy(data, contents.data() + position, readNum);
  return readNum;
}

size_t AssimpIOStream::Write(const void*, size_t, size_t) {
  // Always fails because no writing is allowed
  return 0;
}

aiReturn AssimpIOStream::Seek(size_t offset, aiOrigin origin) {
  switch (origin) {
    case aiOrigin_SET:
      position = offset;
      return aiReturn_SUCCESS;
    case aiOrigin_CUR:
      position += offset;
      return aiReturn_SUCCESS;
    case aiOrigin_END:
      position = FileSize() + offset;
      return aiReturn_SUCCESS;
    default:
      return aiReturn_FAILURE;
  }
}

size_t AssimpIOStream::Tell() const { return position; }

size_t AssimpIOStream::FileSize() const { return contents.size(); }

void AssimpIOStream::Flush() {
  // We're not allowing writing, so flushing is not necessary
}
