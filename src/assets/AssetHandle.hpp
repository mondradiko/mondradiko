#pragma once

template<class T>
class AssetHandle
{
public:
    AssetHandle();
    AssetHandle(T* ptr);
    ~AssetHandle();

    T* operator->() { return ptr; }
    operator bool() { return ptr; }
    void operator=(const AssetHandle<T>&);
private:
    T* ptr;
};

template<class T>
AssetHandle<T>::AssetHandle()
 : ptr(nullptr)
{

}

template<class T>
AssetHandle<T>::AssetHandle(T* ptr)
 : ptr(ptr)
{
    ptr->refCount++;
}

template<class T>
AssetHandle<T>::~AssetHandle()
{
    if(ptr) ptr->refCount--;
}

template<class T>
void AssetHandle<T>::operator=(const AssetHandle<T>& other)
{
    if(ptr) ptr->refCount--;
    ptr = other.ptr;
    ptr->refCount++;
}
