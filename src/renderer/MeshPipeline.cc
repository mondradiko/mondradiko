/**
 * @file MeshPipeline.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Creates and renders MeshRendererComponents, and all its required
 * assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "renderer/MeshPipeline.h"

#include <vector>

#include "gpu/GpuInstance.h"
#include "gpu/GpuShader.h"
#include "log/log.h"
#include "renderer/Renderer.h"
#include "shaders/mesh.frag.h"
#include "shaders/mesh.vert.h"

namespace mondradiko {

MeshPipeline::MeshPipeline(GpuInstance* gpu, VkPipelineLayout pipeline_layout,
                           VkRenderPass render_pass, uint32_t subpass_index)
    : pipeline_layout(pipeline_layout), gpu(gpu) {
  log_zone;

  createPipeline(render_pass, subpass_index);
  createTextureSampler();
  createMaterialBuffer();
}

MeshPipeline::~MeshPipeline() {
  log_zone;

  if (material_buffer != nullptr) delete material_buffer;
  if (texture_sampler != VK_NULL_HANDLE)
    vkDestroySampler(gpu->device, texture_sampler, nullptr);
  if (pipeline != VK_NULL_HANDLE)
    vkDestroyPipeline(gpu->device, pipeline, nullptr);
}

void MeshPipeline::updateDescriptors(VkDescriptorSet descriptors) {
  log_zone;

  std::vector<VkDescriptorImageInfo> texture_infos;

  for (auto& iter : texture_pool.pool) {
    auto& texture = iter.second;

    texture->index = texture_infos.size();
    texture_infos.push_back(VkDescriptorImageInfo{
        .sampler = texture->sampler,
        .imageView = texture->image->view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
  }

  std::vector<VkWriteDescriptorSet> descriptor_writes;

  descriptor_writes.push_back(VkWriteDescriptorSet{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .dstSet = descriptors,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorCount = static_cast<uint32_t>(texture_infos.size()),
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .pImageInfo = texture_infos.data()});

  std::vector<MaterialUniform> materials(64);
  uint32_t material_index = 0;

  for (auto& iter : material_pool.pool) {
    auto& material = iter.second;

    material->index = material_index;
    material->updateUniform(&materials[material_index]);
    material_index++;
  }

  material_buffer->writeData(materials.data());

  VkDescriptorBufferInfo material_info{.buffer = material_buffer->buffer,
                                       .offset = 0,
                                       .range = material_buffer->buffer_size};

  descriptor_writes.push_back(
      VkWriteDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                           .dstSet = descriptors,
                           .dstBinding = 2,
                           .dstArrayElement = 0,
                           .descriptorCount = 1,
                           .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           .pBufferInfo = &material_info});

  vkUpdateDescriptorSets(gpu->device, descriptor_writes.size(),
                         descriptor_writes.data(), 0, nullptr);
}

void MeshPipeline::render(VkCommandBuffer commandBuffer) {
  log_zone;

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  for (auto mesh_renderer : mesh_renderers) {
    // TODO(marceline-cramer) Send material indices in some other way.
    vkCmdPushConstants(
        commandBuffer, pipeline_layout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        offsetof(FramePushConstant, material_index), sizeof(uint32_t),
        &mesh_renderer->material_asset->index);
    VkBuffer vertex_buffers[] = {
        mesh_renderer->mesh_asset->vertex_buffer->buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer,
                         mesh_renderer->mesh_asset->index_buffer->buffer, 0,
                         VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(commandBuffer, mesh_renderer->mesh_asset->index_count, 1,
                     0, 0, 0);
  }
}

MeshRendererComponent* MeshPipeline::createMeshRenderer(
    std::string filename, const aiScene* model_scene, uint32_t mesh_index) {
  log_zone;

  aiMesh* mesh = model_scene->mMeshes[mesh_index];
  auto mesh_asset = loadMesh(filename, model_scene, mesh_index);
  auto material_asset =
      loadMaterial(filename, model_scene, mesh->mMaterialIndex);

  return new MeshRendererComponent(this, mesh_asset, material_asset);
}

AssetHandle<MaterialAsset> MeshPipeline::loadMaterial(
    std::string filename, const aiScene* model_scene, uint32_t material_index) {
  log_zone;

  aiMaterial* material = model_scene->mMaterials[material_index];
  std::string material_name =
      filename + '/' + std::string(material->GetName().C_Str());

  auto cached_material = material_pool.findCached(material_name);

  if (!cached_material) {
    cached_material = material_pool.load(
        material_name,
        new MaterialAsset(this, filename, model_scene, material));
  }

  return cached_material;
}

AssetHandle<MeshAsset> MeshPipeline::loadMesh(std::string filename,
                                              const aiScene* model_scene,
                                              uint32_t mesh_index) {
  log_zone;

  aiMesh* mesh = model_scene->mMeshes[mesh_index];
  std::string mesh_name = filename + '/' + std::string(mesh->mName.C_Str());

  auto cached_mesh = mesh_pool.findCached(mesh_name);

  if (!cached_mesh) {
    cached_mesh =
        mesh_pool.load(mesh_name, new MeshAsset(mesh_name, gpu, mesh));
  }

  return cached_mesh;
}

AssetHandle<TextureAsset> MeshPipeline::loadTexture(std::string filename,
                                                    const aiScene* model_scene,
                                                    aiString texture_string) {
  log_zone;
  log_dbg(texture_string.C_Str());

  // If the texture is embedded, it'll begin with a '*'
  if (texture_string.C_Str()[0] == '*') {
    // The rest of the string contains the index
    uint32_t texture_index = atoi(texture_string.C_Str() + 1);

    aiTexture* texture = model_scene->mTextures[texture_index];
    std::ostringstream texture_format;
    texture_format << filename + '/';
    texture_format << texture->mFilename.C_Str() << "_";
    texture_format << texture_index;

    std::string textureName = texture_format.str();
    log_err(textureName.c_str());

    auto cached_texture = texture_pool.findCached(textureName);

    if (!cached_texture) {
      cached_texture = texture_pool.load(
          textureName, new TextureAsset(gpu, texture, texture_sampler));
    }

    return cached_texture;
  } else {
    log_ftl("Unable to load non-embedded textures.");
    return AssetHandle<TextureAsset>(nullptr);
  }
}

void MeshPipeline::createPipeline(VkRenderPass render_pass, uint32_t subpass) {
  log_zone;

  GpuShader vert_shader(gpu, VK_SHADER_STAGE_VERTEX_BIT, shaders_mesh_vert,
                        sizeof(shaders_mesh_vert));
  GpuShader frag_shader(gpu, VK_SHADER_STAGE_FRAGMENT_BIT, shaders_mesh_frag,
                        sizeof(shaders_mesh_frag));

  std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {
      vert_shader.getStageCreateInfo(), frag_shader.getStageCreateInfo()};

  auto binding_description = MeshVertex::getBindingDescription();
  auto attribute_descriptions = MeshVertex::getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertex_input_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = 1,
      .pVertexBindingDescriptions = &binding_description,
      .vertexAttributeDescriptionCount = attribute_descriptions.size(),
      .pVertexAttributeDescriptions = attribute_descriptions.data()};

  VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
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

  VkPipelineViewportStateCreateInfo viewport_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .pViewports = &viewport,
      .scissorCount = 1,
      .pScissors = &scissor};

  VkPipelineRasterizationStateCreateInfo rasterization_info{
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

  VkPipelineMultisampleStateCreateInfo multisample_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 1.0f,
      .pSampleMask = nullptr,
      .alphaToCoverageEnable = VK_FALSE,
      .alphaToOneEnable = VK_FALSE};

  VkPipelineColorBlendAttachmentState color_blend_attachment{
      .blendEnable = VK_FALSE,
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

  VkPipelineColorBlendStateCreateInfo color_blend_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = &color_blend_attachment};

  std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT,
                                                VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamic_state_info{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
      .pDynamicStates = dynamic_states.data()};

  VkGraphicsPipelineCreateInfo pipeline_info{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .stageCount = static_cast<uint32_t>(shader_stages.size()),
      .pStages = shader_stages.data(),
      .pVertexInputState = &vertex_input_info,
      .pInputAssemblyState = &input_assembly_info,
      .pViewportState = &viewport_info,
      .pRasterizationState = &rasterization_info,
      .pMultisampleState = &multisample_info,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &color_blend_info,
      .pDynamicState = &dynamic_state_info,
      .layout = pipeline_layout,
      .renderPass = render_pass,
      .subpass = subpass,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1};

  if (vkCreateGraphicsPipelines(gpu->device, VK_NULL_HANDLE, 1, &pipeline_info,
                                nullptr, &pipeline) != VK_SUCCESS) {
    log_ftl("Failed to create mesh pipeline.");
  }
}

void MeshPipeline::createTextureSampler() {
  log_zone;

  VkSamplerCreateInfo sampler_info{
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .mipLodBias = 0.0f,
      // TODO(marceline-cramer) Anisotropy support
      .anisotropyEnable = VK_FALSE,
      .compareEnable = VK_FALSE,
      .minLod = 0.0f,
      .maxLod = 0.0f,
      .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
      .unnormalizedCoordinates = VK_FALSE};

  if (vkCreateSampler(gpu->device, &sampler_info, nullptr, &texture_sampler) !=
      VK_SUCCESS) {
    log_ftl("Failed to create texture sampler.");
  }
}

void MeshPipeline::createMaterialBuffer() {
  log_zone;

  material_buffer = new GpuBuffer(gpu, 128 * sizeof(MaterialUniform),
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                  VMA_MEMORY_USAGE_CPU_TO_GPU);
}

}  // namespace mondradiko
