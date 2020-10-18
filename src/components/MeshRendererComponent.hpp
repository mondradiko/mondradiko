#pragma once

#include "api_headers.hpp"

#include "assets/AssetHandle.hpp"
#include "components/Component.hpp"

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
