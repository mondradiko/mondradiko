/**
 * @file Filesystem.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides access to the read-only files in an asset archive.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/filesystem/Filesystem.h"

#include <sstream>

#include "core/log/log.h"

namespace mondradiko {

Filesystem::Filesystem(const char* archive) : archive(archive) {
  log_dbg("Mounting filesystem from path:");
  log_dbg(archive);

  // TODO(marceline-cramer) Add different mount targets for mods/DLC/etc and
  // connect it to this class
  if (!PHYSFS_mount(archive, NULL, 0)) {
    log_ftl("Failed to mount archive.");
  }
}

Filesystem::~Filesystem() { PHYSFS_unmount(archive); }

bool Filesystem::exists(const char* fileName) {
  return PHYSFS_exists(fileName);
}

bool Filesystem::loadBinaryFile(const char* filename,
                                std::vector<unsigned char>* buffer) {
  std::ostringstream infoMessage;
  infoMessage << "Loading file '" << filename << "'.";
  log_inf(infoMessage.str().c_str());

  if (!PHYSFS_exists(filename)) {
    std::ostringstream errorMessage;
    errorMessage << "File '" << filename << "' does not exist.";
    log_err(errorMessage.str().c_str());
    return false;
  }

  PHYSFS_file* f = PHYSFS_openRead(filename);

  if (!f) {
    std::ostringstream errorMessage;
    errorMessage << "Failed to open file '" << filename << "'.";
    log_err(errorMessage.str().c_str());
    return false;
  }

  buffer->resize(PHYSFS_fileLength(f));
  PHYSFS_readBytes(f, buffer->data(), buffer->size());
  PHYSFS_close(f);

  return true;
}

}  // namespace mondradiko
