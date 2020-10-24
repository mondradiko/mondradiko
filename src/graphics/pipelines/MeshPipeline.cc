/**
 * @file MeshPipeline.cc
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

#include "graphics/pipelines/MeshPipeline.h"

#include <vector>

#include "graphics/VulkanInstance.h"
#include "graphics/shaders/MeshShader.h"
#include "log/log.h"

MeshPipeline::MeshPipeline(VulkanInstance* _vulkanInstance,
                           VkDescriptorSetLayout cameraSetLayout,
                           VkRenderPass renderPass, uint32_t subpassIndex) {
  log_dbg("Creating mesh pipeline.");

  vulkanInstance = _vulkanInstance;

  createPipelineLayout(cameraSetLayout);
  createPipeline(renderPass, subpassIndex);
}

MeshPipeline::~MeshPipeline() {
  log_dbg("Destroying mesh pipeline.");

  if (pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(vulkanInstance->device, pipeline, nullptr);
  if (pipelineLayout != VK_NULL_HANDLE)
    vkDestroyPipelineLayout(vulkanInstance->device, pipelineLayout, nullptr);
}

void MeshPipeline::render(VkCommandBuffer commandBuffer) {
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  for (auto meshRenderer : meshRenderers) {
    VkBuffer vertexBuffers[] = {meshRenderer->meshAsset->vertexBuffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer,
                         meshRenderer->meshAsset->indexBuffer->buffer, 0,
                         VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, meshRenderer->meshAsset->indexCount, 1, 0,
                     0, 0);
  }
}

MeshRendererComponent* MeshPipeline::createMeshRenderer(
    std::string fileName, const aiScene* modelScene, uint32_t meshIndex) {
  aiMesh* mesh = modelScene->mMeshes[meshIndex];
  auto meshAsset = loadMesh(fileName, modelScene, meshIndex);
  auto materialAsset = loadMaterial(fileName, modelScene, mesh->mMaterialIndex);

  return new MeshRendererComponent(this, meshAsset, materialAsset);
}

AssetHandle<MaterialAsset> MeshPipeline::loadMaterial(std::string fileName,
                                                      const aiScene* modelScene,
                                                      uint32_t materialIndex) {
  aiMaterial* material = modelScene->mMaterials[materialIndex];
  std::string materialName =
      fileName + '/' + std::string(material->GetName().C_Str());

  auto cachedMaterial = materialAssets.findCached(materialName);

  if (!cachedMaterial) {
    cachedMaterial = materialAssets.load(
        materialName, new MaterialAsset(this, fileName, modelScene, material));
  }

  return cachedMaterial;
}

AssetHandle<MeshAsset> MeshPipeline::loadMesh(std::string fileName,
                                              const aiScene* modelScene,
                                              uint32_t meshIndex) {
  aiMesh* mesh = modelScene->mMeshes[meshIndex];
  std::string meshName = fileName + '/' + std::string(mesh->mName.C_Str());

  auto cachedMesh = meshAssets.findCached(meshName);

  if (!cachedMesh) {
    cachedMesh = meshAssets.load(meshName,
                                 new MeshAsset(meshName, vulkanInstance, mesh));
  }

  return cachedMesh;
}

AssetHandle<TextureAsset> MeshPipeline::loadTexture(std::string fileName,
                                                    const aiScene* modelScene,
                                                    aiString textureString) {
  log_dbg(textureString.C_Str());

  // If the texture is embedded, it'll begin with a '*'
  if (textureString.C_Str()[0] == '*') {
    // The rest of the string contains the index
    uint32_t textureIndex = atoi(textureString.C_Str() + 1);

    aiTexture* texture = modelScene->mTextures[textureIndex];
    std::ostringstream textureFormat;
    textureFormat << fileName + '/';
    textureFormat << texture->mFilename.C_Str() << "_";
    textureFormat << textureIndex;

    std::string textureName = textureFormat.str();
    log_err(textureName.c_str());

    auto cachedTexture = textureAssets.findCached(textureName);

    if (!cachedTexture) {
      cachedTexture = textureAssets.load(
          textureName, new TextureAsset(vulkanInstance, texture));
    }

    return cachedTexture;
  } else {
    log_ftl("Unable to load non-embedded textures.");
    return nullptr;
  }
}

void MeshPipeline::createPipelineLayout(VkDescriptorSetLayout cameraSetLayout) {
  VkPipelineLayoutCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &cameraSetLayout,
      .pushConstantRangeCount = 0};

  if (vkCreatePipelineLayout(vulkanInstance->device, &createInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    log_ftl("Failed to create mesh pipeline layout.");
  }
}

void MeshPipeline::createPipeline(VkRenderPass renderPass, uint32_t subpass) {
  MeshShader shader(vulkanInstance);
  auto shaderStages = shader.getStages();

  auto bindingDescription = MeshVertex::getBindingDescription();
  auto attributeDescriptions = MeshVertex::getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &bindingDescription,
      .vertexAttributeDescriptionCount = attributeDescriptions.size(),
      .pVertexAttributeDescriptions = attributeDescriptions.data()};

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  // TODO(marceline-cramer) Get viewport state from Viewport
  VkViewport viewport{.x = 0.0f,
                      .y = 0.0f,
                      .width = static_cast<float>(500),
                      .height = static_cast<float>(500),
                      .minDepth = 0.0f,
                      .maxDepth = 1.0f};

  VkRect2D scissor{.offset = {0, 0}, .extent = {500, 500}};

  VkPipelineViewportStateCreateInfo viewportState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor};

  VkPipelineRasterizationStateCreateInfo rasterizationState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f};

  VkPipelineMultisampleStateCreateInfo multisampling{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE};

  VkPipelineColorBlendAttachmentState colorBlendAttachment{
      .blendEnable = VK_FALSE,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  VkPipelineColorBlendStateCreateInfo colorBlending{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachment};

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data()};

  VkGraphicsPipelineCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = static_cast<uint32_t>(shaderStages.size()),
      .pStages = shaderStages.data(),
      .pVertexInputState = &vertexInputInfo,
      .pInputAssemblyState = &inputAssembly,
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterizationState,
      .pMultisampleState = &multisampling,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &colorBlending,
      .pDynamicState = &dynamicState,
      .layout = pipelineLayout,
      .renderPass = renderPass,
      .subpass = subpass,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1};

  if (vkCreateGraphicsPipelines(vulkanInstance->device, VK_NULL_HANDLE, 1,
                                &createInfo, nullptr,
                                &pipeline) != VK_SUCCESS) {
    log_ftl("Failed to create mesh pipeline.");
  }
}
