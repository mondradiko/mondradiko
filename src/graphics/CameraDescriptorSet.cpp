#include "graphics/CameraDescriptorSet.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"

CameraDescriptorSet::CameraDescriptorSet(VulkanInstance* vulkanInstance)
 : vulkanInstance(vulkanInstance)
{
    VkDescriptorSetLayoutBinding uboBinding{
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
    };

    VkDescriptorSetLayoutCreateInfo setLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &uboBinding
    };

    if(vkCreateDescriptorSetLayout(vulkanInstance->device, &setLayoutInfo, nullptr, &setLayout) != VK_SUCCESS) {
        log_ftl("Failed to create camera descriptor set layout.");
    }

    VkDescriptorSetAllocateInfo setInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vulkanInstance->descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &setLayout
    };

    if(vkAllocateDescriptorSets(vulkanInstance->device, &setInfo, &descriptorSet) != VK_SUCCESS) {
        log_ftl("Failed to allocate camera descriptor set.");
    }

    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(CameraUniform),
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo allocationInfo{
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU
    };

    if(vmaCreateBuffer(vulkanInstance->allocator, &bufferInfo, &allocationInfo, &uniformBuffer, &uniformAllocation, &uniformAllocationInfo) != VK_SUCCESS) {
        log_ftl("Failed to allocate camera uniform buffer.");
    }

    VkDescriptorBufferInfo bufferDescriptorInfo{
        .buffer = uniformBuffer,
        .offset = 0,
        .range = sizeof(CameraUniform)
    };

    VkWriteDescriptorSet descriptorWrite{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferDescriptorInfo
    };

    vkUpdateDescriptorSets(vulkanInstance->device, 1, &descriptorWrite, 0, nullptr);
}

CameraDescriptorSet::~CameraDescriptorSet()
{
    if(uniformAllocation != nullptr) vmaDestroyBuffer(vulkanInstance->allocator, uniformBuffer, uniformAllocation);

    if(setLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(vulkanInstance->device, setLayout, nullptr);
}

void CameraDescriptorSet::update()
{
    CameraUniform ubo;
    ubo.view = glm::mat4(1.0);
    ubo.projection = glm::mat4(1.0);

    void* data;
    vkMapMemory(vulkanInstance->device, uniformAllocationInfo.deviceMemory, uniformAllocationInfo.offset, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vulkanInstance->device, uniformAllocationInfo.deviceMemory);
}

void CameraDescriptorSet::bind(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}
