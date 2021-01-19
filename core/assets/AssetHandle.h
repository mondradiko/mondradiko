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
    _ref();
  }

  explicit AssetHandle(const AssetHandle<AssetType>& other) {
    id = other.id;
    ptr = other.ptr;
    _ref();
  }

  explicit AssetHandle(AssetHandle<AssetType>&& other) {
    id = other.id;
    ptr = other.ptr;
    _ref();
  }

  AssetHandle<AssetType>& operator=(const AssetHandle<AssetType>& other) {
    _unref();
    id = other.id;
    ptr = other.ptr;
    _ref();
    return *this;
  }

  AssetHandle<AssetType>& operator=(AssetHandle<AssetType>&& other) {
    _unref();
    id = other.id;
    ptr = other.ptr;
    _ref();
    return *this;
  }

  ~AssetHandle() { _unref(); }

  const AssetType* operator->() const {
    if (!isLoaded()) {
      const char* type_name = assets::EnumNameAssetType(AssetType::ASSET_TYPE);
      log_err("Operating on unloaded %s", type_name);
    }

    return ptr;
  }

  operator bool() const { return isLoaded(); }

  AssetId getId() const {
    if (!isLoaded()) return AssetId::NullAsset;
    return id;
  }

  bool isLoaded() const {
    if (ptr != nullptr) return ptr->isLoaded();
    return false;
  }

 private:
  AssetId id;
  AssetType* ptr;

  void _ref() {
    return;
    if (ptr != nullptr) {
      ptr->ref_count++;
    }
  }

  void _unref() {
    return;
    if (ptr != nullptr) {
      ptr->ref_count--;
    }
  }
};

}  // namespace mondradiko
