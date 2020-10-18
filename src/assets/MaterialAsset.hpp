#pragma once

#include "api_headers.hpp"
#include "assets/Asset.hpp"

class MaterialAsset : public Asset
{
public:
    MaterialAsset(const aiScene*, aiMaterial*);
private:
};
