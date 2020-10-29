/**
 * @file AssetPool.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Holds Assets of a given type, caching by filename,
 * and automatically freeing on expiration.
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

#ifndef SRC_ASSETS_ASSETPOOL_H_
#define SRC_ASSETS_ASSETPOOL_H_

#include <unordered_map>
#include <string>

namespace mondradiko {

template<class T>
class AssetPool {
 public:
  ~AssetPool();

  AssetHandle<T> findCached(std::string);
  AssetHandle<T> load(std::string, T*);
 private:
  std::unordered_map<std::string, T*> pool;
};

template<class T>
AssetPool<T>::~AssetPool() {
    for (const auto& asset : pool) {
        delete asset.second;
    }
}

template<class T>
AssetHandle<T> AssetPool<T>::findCached(std::string key) {
    auto it = pool.find(key);

    if (it == pool.end()) {
        return AssetHandle<T>();
    }

    return AssetHandle<T>(it->second);
}

// TODO(marceline-cramer) Use argument templates to initialize in-function
template<class T>
AssetHandle<T> AssetPool<T>::load(std::string key, T* newAsset) {
    pool.emplace(key, newAsset);
    return AssetHandle<T>(newAsset);
}

}  // namespace mondradiko

#endif  // SRC_ASSETS_ASSETPOOL_H_
