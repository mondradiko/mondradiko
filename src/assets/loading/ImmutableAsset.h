/**
 * @file ImmutableAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */
#pragma once

#include "assets/format/AssetTypes.h"

namespace mondradiko {
namespace assets {

class ImmutableAsset {
 public:
  ImmutableAsset(const void* data, size_t data_size)
      : data(data), cursor(data), data_size(data_size) {}
  
  template<class T>
  const ImmutableAsset& operator>>(T&);

 private:
  const void* data;
  const void* cursor;
  size_t data_size;
};

template <class T>
const ImmutableAsset& ImmutableAsset::operator>>(T& other) {
  other = *static_cast<T*>(cursor);
  cursor += sizeof(T);
  return *this;
}

}  // namespace assets
}  // namespace mondradiko
