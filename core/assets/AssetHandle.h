/**
 * @file AssetHandle.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief References an Asset.
 * @date 2021-16-01
 *
 * @copyright Copyright (c) 2020 the Mondradiko contributors.
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include "core/assets/Asset.h"
#include "log/log.h"

namespace mondradiko {

template <class AssetType>
class AssetHandle {
 public:
  AssetHandle() : ptr(nullptr) {}

  explicit AssetHandle(std::nullptr_t ptr) : id(AssetId::NullAsset), ptr(ptr) {}

  explicit AssetHandle(AssetId id, AssetType* ptr) : id(id), ptr(ptr) {
    ptr->ref_count++;
  }

  explicit AssetHandle(const AssetHandle<AssetType>& other) {
    id = other.id;
    ptr = other.ptr;

    if (ptr) ptr->ref_count++;
  }

  AssetHandle<AssetType>& operator=(const AssetHandle<AssetType>& other) {
    if (ptr) ptr->ref_count--;

    id = other.id;
    ptr = other.ptr;

    if (ptr) ptr->ref_count++;

    return *this;
  }

  ~AssetHandle() {
    if (ptr) ptr->ref_count--;
  }

  const AssetType* operator->() const {
    if (!isLoaded()) {
      const char* type_name = assets::EnumNameAssetType(AssetType::ASSET_TYPE);
      log_wrn("Operating on unloaded %s", type_name);
    }

    return ptr;
  }

  operator bool() const { return isLoaded(); }

  AssetId getId() const {
    if (!isLoaded()) return AssetId::NullAsset;
    return id;
  }

  bool isLoaded() const { return ptr && ptr->isLoaded(); }

 private:
  AssetId id;
  AssetType* ptr;
};

}  // namespace mondradiko
