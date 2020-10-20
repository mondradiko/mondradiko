#pragma once

#include <vector>

#include "api_headers.hpp"
#include "assets/Asset.hpp"
#include "assets/AssetHandle.hpp"
#include "assets/TextureAsset.hpp"

class MeshPipeline;

class MaterialAsset : public Asset
{
public:
    MaterialAsset(MeshPipeline*, std::string, const aiScene*, aiMaterial*);
    ~MaterialAsset();

    AssetHandle<TextureAsset> textureBaseColor;
private:
    MeshPipeline* meshPipeline;
};
