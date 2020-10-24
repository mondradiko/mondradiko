/**
 * @file MeshPipeline.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and renders MeshRendererComponents, and all its required
 * assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
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

class MeshPipeline {
 public:
  MeshPipeline(VulkanInstance*, VkDescriptorSetLayout, VkRenderPass, uint32_t);
  ~MeshPipeline();

  void render(VkCommandBuffer);

  MeshRendererComponent* createMeshRenderer(std::string, const aiScene*,
                                            uint32_t);

  AssetHandle<MaterialAsset> loadMaterial(std::string, const aiScene*,
                                          uint32_t);
  AssetHandle<MeshAsset> loadMesh(std::string, const aiScene*, uint32_t);
  AssetHandle<TextureAsset> loadTexture(std::string, const aiScene*, aiString);

  VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;

  std::set<MeshRendererComponent*> meshRenderers;

 private:
  VulkanInstance* vulkanInstance;

  AssetPool<MaterialAsset> materialAssets;
  AssetPool<MeshAsset> meshAssets;
  AssetPool<TextureAsset> textureAssets;

  void createPipelineLayout(VkDescriptorSetLayout);
  void createPipeline(VkRenderPass, uint32_t);
};

#endif  // SRC_GRAPHICS_PIPELINES_MESHPIPELINE_H_
