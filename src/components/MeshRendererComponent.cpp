#include "components/MeshRendererComponent.hpp"

#include "assets/MaterialAsset.hpp"
#include "assets/MeshAsset.hpp"
#include "graphics/pipelines/MeshPipeline.hpp"

MeshRendererComponent::MeshRendererComponent(MeshPipeline* meshPipeline, AssetHandle<MeshAsset>& meshAsset, AssetHandle<MaterialAsset>& materialAsset)
 : meshAsset(meshAsset),
   materialAsset(materialAsset),
   meshPipeline(meshPipeline)
{

}
