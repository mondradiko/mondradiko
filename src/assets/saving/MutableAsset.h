/**
 * @file MutableAsset.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief
 * @date 2020-12-12
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <cstring>
#include <vector>

#include "assets/format/AssetTypes.h"

namespace mondradiko {
namespace assets {

class MutableAsset {
 public:
  template <class T>
  MutableAsset& operator<<(const T&);

  friend class AssetBundleBuilder;

 private:
  std::vector<char> data;
};

template <class T>
MutableAsset& MutableAsset::operator<<(const T& other) {
  size_t append_ptr = data.size();
  data.resize(append_ptr + sizeof(T));
  memcpy(data.data() + append_ptr, reinterpret_cast<const void*>(&other),
         sizeof(T));
  return *this;
}

}  // namespace assets
}  // namespace mondradiko
