#include "assets/MaterialAsset.hpp"

#include "graphics/pipelines/MeshPipeline.hpp"

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
