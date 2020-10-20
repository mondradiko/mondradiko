#include "assets/MaterialAsset.h"

#include "graphics/pipelines/MeshPipeline.h"

MaterialAsset::MaterialAsset(MeshPipeline* meshPipeline, std::string fileName, const aiScene* modelScene, aiMaterial* material)
 : meshPipeline(meshPipeline)
{
    aiString baseColor;
    material->GetTexture(aiTextureType_DIFFUSE, 0, &baseColor);
    textureBaseColor = meshPipeline->loadTexture(fileName, modelScene, baseColor);
}

MaterialAsset::~MaterialAsset()
{

}
