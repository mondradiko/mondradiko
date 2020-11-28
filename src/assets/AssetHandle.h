/**
 * @file AssetHandle.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements reference counting of Assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_ASSETS_ASSETHANDLE_H_
#define SRC_ASSETS_ASSETHANDLE_H_

namespace mondradiko {

template <class T>
class AssetHandle {
 public:
  AssetHandle();
  explicit AssetHandle(T* ptr);
  ~AssetHandle();

  T* operator->() { return ptr; }
  operator bool() { return ptr; }
  void operator=(const AssetHandle<T>&);

 private:
  T* ptr;
};

template <class T>
AssetHandle<T>::AssetHandle() : ptr(nullptr) {}

template <class T>
AssetHandle<T>::AssetHandle(T* ptr) : ptr(ptr) {
  ptr->ref_count++;
}

template <class T>
AssetHandle<T>::~AssetHandle() {
  if (ptr) ptr->ref_count--;
}

template <class T>
void AssetHandle<T>::operator=(const AssetHandle<T>& other) {
  if (ptr) ptr->ref_count--;
  ptr = other.ptr;
  ptr->ref_count++;
}

}  // namespace mondradiko

#endif  // SRC_ASSETS_ASSETHANDLE_H_
