/**
 * @file MeshPipeline.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and renders MeshRendererComponents, and all its required
 * assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_GRAPHICS_PIPELINES_MESHPIPELINE_H_
#define SRC_GRAPHICS_PIPELINES_MESHPIPELINE_H_

#include <map>
#include <set>
#include <string>

#include "assets/AssetHandle.h"
#include "assets/AssetPool.h"
#include "assets/MaterialAsset.h"
#include "assets/MeshAsset.h"
#include "assets/TextureAsset.h"
#include "components/MeshRendererComponent.h"
#include "graphics/VulkanInstance.h"
#include "src/api_headers.h"

namespace mondradiko {

class MeshPipeline {
 public:
  MeshPipeline(VulkanInstance*, VkPipelineLayout, VkRenderPass, uint32_t);
  ~MeshPipeline();

  void updateDescriptors(VkDescriptorSet);
  void render(VkCommandBuffer);

  MeshRendererComponent* createMeshRenderer(std::string, const aiScene*,
                                            uint32_t);

  AssetHandle<MaterialAsset> loadMaterial(std::string, const aiScene*,
                                          uint32_t);
  AssetHandle<MeshAsset> loadMesh(std::string, const aiScene*, uint32_t);
  AssetHandle<TextureAsset> loadTexture(std::string, const aiScene*, aiString);

  VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  VkSampler texture_sampler = VK_NULL_HANDLE;

  std::set<MeshRendererComponent*> mesh_renderers;

  VulkanBuffer* material_buffer = nullptr;

 private:
  VulkanInstance* vulkan_instance;

  AssetPool<MaterialAsset> material_pool;
  AssetPool<MeshAsset> mesh_pool;
  AssetPool<TextureAsset> texture_pool;

  void createPipeline(VkRenderPass, uint32_t);
  void createTextureSampler();
  void createMaterialBuffer();
};

}  // namespace mondradiko

#endif  // SRC_GRAPHICS_PIPELINES_MESHPIPELINE_H_