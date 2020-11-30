/**
 * @file MeshPipeline.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and renders MeshRendererComponents, and all its required
 * assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_RENDERER_MESHPIPELINE_H_
#define SRC_RENDERER_MESHPIPELINE_H_

#include <map>
#include <set>
#include <string>

#include "assets/AssetHandle.h"
#include "assets/AssetPool.h"
#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "assets/TextureAsset.h"
#include "components/MeshRendererComponent.h"
#include "gpu/GpuInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class MeshPipeline {
 public:
  MeshPipeline(GpuInstance*, VkDescriptorSetLayout, VkRenderPass, uint32_t);
  ~MeshPipeline();

  void updateDescriptors(VkDescriptorSet);
  void render(VkCommandBuffer, VkDescriptorSet, uint32_t);

  MeshRendererComponent* createMeshRenderer(std::string, const aiScene*,
                                            uint32_t);

  AssetHandle<MaterialAsset> loadMaterial(std::string, const aiScene*,
                                          uint32_t);
  AssetHandle<MeshAsset> loadMesh(std::string, const aiScene*, uint32_t);
  AssetHandle<TextureAsset> loadTexture(std::string, const aiScene*, aiString);

  VkDescriptorSetLayout material_layout = VK_NULL_HANDLE;

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkSampler texture_sampler = VK_NULL_HANDLE;

  std::set<MeshRendererComponent*> mesh_renderers;

  GpuBuffer* material_buffer = nullptr;

 private:
  GpuInstance* gpu;

  AssetPool<MaterialAsset> material_pool;
  AssetPool<MeshAsset> mesh_pool;
  AssetPool<TextureAsset> texture_pool;

  void createSetLayouts();
  void createPipelineLayout(VkDescriptorSetLayout);
  void createPipeline(VkRenderPass, uint32_t);
  void createTextureSampler();
  void createMaterialBuffer();
};

}  // namespace mondradiko

#endif  // SRC_RENDERER_MESHPIPELINE_H_
