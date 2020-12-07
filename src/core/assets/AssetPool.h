/**
 * @file AssetPool.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Holds Assets of a given type, caching by filename,
 * and automatically freeing on expiration.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <string>
#include <unordered_map>

namespace mondradiko {

template <class T>
class AssetPool {
 public:
  ~AssetPool();

  AssetHandle<T> findCached(std::string);
  AssetHandle<T> load(std::string, T*);

  std::unordered_map<std::string, T*> pool;
};

template <class T>
AssetPool<T>::~AssetPool() {
  for (const auto& asset : pool) {
    delete asset.second;
  }
}

template <class T>
AssetHandle<T> AssetPool<T>::findCached(std::string key) {
  auto it = pool.find(key);

  if (it == pool.end()) {
    return AssetHandle<T>();
  }

  return AssetHandle<T>(it->second);
}

// TODO(marceline-cramer) Use argument templates to initialize in-function
template <class T>
AssetHandle<T> AssetPool<T>::load(std::string key, T* newAsset) {
  pool.emplace(key, newAsset);
  return AssetHandle<T>(newAsset);
}

}  // namespace mondradiko
