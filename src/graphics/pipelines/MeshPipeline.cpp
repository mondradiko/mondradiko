#include "graphics/pipelines/MeshPipeline.hpp"

#include "assets/MeshAsset.hpp"
#include "graphics/VulkanInstance.hpp"
#include "graphics/shaders/MeshShader.hpp"
#include "log/log.hpp"

MeshPipeline::MeshPipeline(VulkanInstance* _vulkanInstance, VkDescriptorSetLayout cameraSetLayout, VkRenderPass renderPass, uint32_t subpassIndex)
{
    log_dbg("Creating mesh pipeline.");

    vulkanInstance = _vulkanInstance;

    createPipelineLayout(cameraSetLayout);
    createPipeline(renderPass, subpassIndex);
}

MeshPipeline::~MeshPipeline()
{
    log_dbg("Destroying mesh pipeline.");

    for(auto meshAsset : meshAssets) {
        delete meshAsset.second;
    }

    if(pipeline != VK_NULL_HANDLE) vkDestroyPipeline(vulkanInstance->device, pipeline, nullptr);
    if(pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(vulkanInstance->device, pipelineLayout, nullptr);
}

void MeshPipeline::render(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    //render(); lol
}

MeshAsset* MeshPipeline::loadMesh(std::string fileName, aiMesh* mesh)
{
    std::string meshName = fileName + '/' + std::string(mesh->mName.C_Str());
    auto cachedMesh = meshAssets.find(meshName);

    if(cachedMesh == meshAssets.end()) {
        MeshAsset* newMesh = new MeshAsset(meshName, vulkanInstance, mesh);
        meshAssets.emplace(meshName, newMesh);
        return newMesh;
    } else {
        return cachedMesh->second;
    }
}

void MeshPipeline::createPipelineLayout(VkDescriptorSetLayout cameraSetLayout)
{
    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &cameraSetLayout,
        .pushConstantRangeCount = 0
    };

    if(vkCreatePipelineLayout(vulkanInstance->device, &createInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        log_ftl("Failed to create mesh pipeline layout.");
    }
}

void MeshPipeline::createPipeline(VkRenderPass renderPass, uint32_t subpass)
{
    MeshShader shader(vulkanInstance);
    auto shaderStages = shader.getStages();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    // TODO Get viewport state from Viewport
    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) 500,
        .height = (float) 500,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor{
        .offset = {0, 0},
        .extent = {500, 500}
    };

    VkPipelineViewportStateCreateInfo viewportState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

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
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampling{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

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
        .basePipelineIndex = -1
    };

    if(vkCreateGraphicsPipelines(vulkanInstance->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline) != VK_SUCCESS) {
        log_ftl("Failed to create mesh pipeline.");
    }
}
