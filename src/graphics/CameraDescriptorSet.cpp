#include "graphics/CameraDescriptorSet.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"

CameraDescriptorSet::CameraDescriptorSet(VulkanInstance* vulkanInstance, uint32_t viewCount)
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

    descriptorSets.resize(viewCount);
    std::vector<VkDescriptorSetLayout> setLayouts(viewCount, setLayout);

    VkDescriptorSetAllocateInfo setInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = vulkanInstance->descriptorPool,
        .descriptorSetCount = viewCount,
        .pSetLayouts = setLayouts.data()
    };

    if(vkAllocateDescriptorSets(vulkanInstance->device, &setInfo, descriptorSets.data()) != VK_SUCCESS) {
        log_ftl("Failed to allocate camera descriptor set.");
    }

    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(CameraUniform) * viewCount,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo allocationInfo{
        .usage = VMA_MEMORY_USAGE_CPU_TO_GPU
    };

    if(vmaCreateBuffer(vulkanInstance->allocator, &bufferInfo, &allocationInfo, &uniformBuffer, &uniformAllocation, &uniformAllocationInfo) != VK_SUCCESS) {
        log_ftl("Failed to allocate camera uniform buffer.");
    }

    for(uint32_t i = 0; i < viewCount; i++) {
        VkDescriptorBufferInfo bufferDescriptorInfo{
            .buffer = uniformBuffer,
            .offset = sizeof(CameraUniform) * i,
            .range = sizeof(CameraUniform)
        };

        VkWriteDescriptorSet descriptorWrite{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &bufferDescriptorInfo
        };

        vkUpdateDescriptorSets(vulkanInstance->device, 1, &descriptorWrite, 0, nullptr);
    }
}

CameraDescriptorSet::~CameraDescriptorSet()
{
    if(uniformAllocation != nullptr) vmaDestroyBuffer(vulkanInstance->allocator, uniformBuffer, uniformAllocation);

    if(setLayout != VK_NULL_HANDLE) vkDestroyDescriptorSetLayout(vulkanInstance->device, setLayout, nullptr);
}

void CameraDescriptorSet::update(std::vector<XrView>* views)
{
    std::vector<CameraUniform> ubos(views->size());

    for(uint32_t i = 0; i < views->size(); i++) {
        XrView& view = (*views)[i];

        glm::quat viewOrientation = glm::quat(
            view.pose.orientation.w,
            view.pose.orientation.x,
            view.pose.orientation.y,
            view.pose.orientation.z
        );

        glm::vec3 viewPosition = glm::vec3(
            view.pose.position.x,
            view.pose.position.y,
            view.pose.position.z
        );

        ubos[i].view = glm::translate(glm::mat4(viewOrientation), -viewPosition);
        ubos[i].projection = glm::perspective(glm::radians(45.0), 1.0, 0.001, 1000.0);
        ubos[i].projection[1][1] *= -1;
    }

    void* data;
    size_t copySize = sizeof(CameraUniform) * ubos.size();
    vkMapMemory(vulkanInstance->device, uniformAllocationInfo.deviceMemory, uniformAllocationInfo.offset, copySize, 0, &data);
        memcpy(data, ubos.data(), copySize);
    vkUnmapMemory(vulkanInstance->device, uniformAllocationInfo.deviceMemory);
}

void CameraDescriptorSet::bind(VkCommandBuffer commandBuffer, uint32_t viewIndex, VkPipelineLayout pipelineLayout)
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[viewIndex], 0, nullptr);
}
