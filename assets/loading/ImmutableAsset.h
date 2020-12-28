/**
 * @file ImmutableAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */
#pragma once

#include "assets/format/AssetTypes.h"

namespace mondradiko {
namespace assets {

class ImmutableAsset {
 public:
  template <class T>
  ImmutableAsset& operator>>(T&);

  inline const char* getData(size_t* remaining_size) {
    if (remaining_size) {
      *remaining_size = data_size + (data - cursor);
    }

    return cursor;
  }

 private:
  char* data;
  char* cursor;
  size_t data_size;

  friend class AssetLump;
};

template <class T>
ImmutableAsset& ImmutableAsset::operator>>(T& other) {
  other = *reinterpret_cast<T*>(cursor);
  cursor += sizeof(T);
  return *this;
}

}  // namespace assets
}  // namespace mondradiko
