#pragma once

class Asset
{
public:
    virtual ~Asset() {}
private:
    template<class T>
    friend class AssetHandle;

    int refCount = 0;
};
