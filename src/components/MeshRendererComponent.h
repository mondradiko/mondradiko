#pragma once

#include "api_headers.h"

#include "assets/AssetHandle.h"
#include "components/Component.h"

class MaterialAsset;
class MeshAsset;
class MeshPipeline;

class MeshRendererComponent : public Component
{
public:
    MeshRendererComponent(MeshPipeline*, AssetHandle<MeshAsset>&, AssetHandle<MaterialAsset>&);
    ~MeshRendererComponent();

    AssetHandle<MeshAsset> meshAsset;
    AssetHandle<MaterialAsset> materialAsset;
private:
    MeshPipeline* meshPipeline;
};
