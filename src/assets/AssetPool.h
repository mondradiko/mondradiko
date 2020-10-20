#pragma once

#include <unordered_map>
#include <string>

template<class T>
class AssetPool
{
public:
    ~AssetPool();

    AssetHandle<T> findCached(std::string);
    AssetHandle<T> load(std::string, T*);
private:
    std::unordered_map<std::string, T*> pool;
};

template<class T>
AssetPool<T>::~AssetPool()
{
    for(auto asset : pool) {
        delete asset.second;
    }
}

template<class T>
AssetHandle<T> AssetPool<T>::findCached(std::string key)
{
    auto it = pool.find(key);

    if(it == pool.end()) {
        return AssetHandle<T>();
    }

    return AssetHandle<T>(it->second);
}

// TODO Use argument templates to initialize statically
template<class T>
AssetHandle<T> AssetPool<T>::load(std::string key, T* newAsset)
{
    pool.emplace(key, newAsset);
    return AssetHandle<T>(newAsset);
}
