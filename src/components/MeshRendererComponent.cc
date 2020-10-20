#include "components/MeshRendererComponent.h"

#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "graphics/pipelines/MeshPipeline.h"

MeshRendererComponent::MeshRendererComponent(MeshPipeline* meshPipeline, AssetHandle<MeshAsset>& meshAsset, AssetHandle<MaterialAsset>& materialAsset)
 : meshAsset(meshAsset),
   materialAsset(materialAsset),
   meshPipeline(meshPipeline)
{
    meshPipeline->meshRenderers.insert(this);
}

MeshRendererComponent::~MeshRendererComponent()
{
    meshPipeline->meshRenderers.erase(this);
}
