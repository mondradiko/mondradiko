/**
 * @file AssetHandle.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements reference counting of Assets.
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

#ifndef SRC_ASSETS_ASSETHANDLE_H_
#define SRC_ASSETS_ASSETHANDLE_H_

namespace mondradiko {

template<class T>
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

template<class T>
AssetHandle<T>::AssetHandle()
    : ptr(nullptr) {
}

template<class T>
AssetHandle<T>::AssetHandle(T* ptr)
    : ptr(ptr) {
    ptr->refCount++;
}

template<class T>
AssetHandle<T>::~AssetHandle() {
    if (ptr) ptr->refCount--;
}

template<class T>
void AssetHandle<T>::operator=(const AssetHandle<T>& other) {
    if (ptr) ptr->refCount--;
    ptr = other.ptr;
    ptr->refCount++;
}

}  // namespace mondradiko

#endif  // SRC_ASSETS_ASSETHANDLE_H_
