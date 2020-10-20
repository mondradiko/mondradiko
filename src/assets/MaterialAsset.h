#pragma once

#include <vector>

#include "api_headers.h"
#include "assets/Asset.h"
#include "assets/AssetHandle.h"
#include "assets/TextureAsset.h"

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
