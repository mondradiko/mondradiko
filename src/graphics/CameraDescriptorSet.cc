#include "graphics/CameraDescriptorSet.h"

#include "graphics/VulkanBuffer.h"
#include "graphics/VulkanInstance.h"
#include "log/log.h"

static glm::mat4 createProjectionFromFOV(const XrFovf fov, const float near_z, const float far_z)
{
    const float tan_left = tanf(fov.angleLeft);
    const float tan_right = tanf(fov.angleRight);

    const float tan_down = tanf(fov.angleDown);
    const float tan_up = tanf(fov.angleUp);

    const float tan_width = tan_right - tan_left;
    const float tan_height = tan_up - tan_down;

    const float a11 = 2 / tan_width;
    const float a22 = 2 / tan_height;

    const float a31 = (tan_right + tan_left) / tan_width;
    const float a32 = (tan_up + tan_down) / tan_height;
    const float a33 = -far_z / (far_z - near_z);

    const float a43 = -(far_z * near_z) / (far_z - near_z);

    const float mat[16] = {
        a11, 0, 0, 0,
        0, -a22, 0, 0,
        a31, a32, a33, -1,
        0, 0, a43, 0
    };

    return glm::make_mat4(mat);
}

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

    uniformBuffer = new VulkanBuffer(vulkanInstance, viewCount * sizeof(CameraUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    for(uint32_t i = 0; i < viewCount; i++) {
        VkDescriptorBufferInfo bufferDescriptorInfo{
            .buffer = uniformBuffer->buffer,
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
    if(uniformBuffer != nullptr) delete uniformBuffer;

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
        ubos[i].projection = createProjectionFromFOV(view.fov, 0.001, 1000.0);
    }

    uniformBuffer->writeData(ubos.data());
}

void CameraDescriptorSet::bind(VkCommandBuffer commandBuffer, uint32_t viewIndex, VkPipelineLayout pipelineLayout)
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[viewIndex], 0, nullptr);
}
