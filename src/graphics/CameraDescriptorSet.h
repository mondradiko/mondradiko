#pragma once

#include <vector>

#include "api_headers.h"

class VulkanBuffer;
class VulkanInstance;

struct CameraUniform
{
    glm::mat4 view;
    glm::mat4 projection;
};

class CameraDescriptorSet
{
public:
    CameraDescriptorSet(VulkanInstance*, uint32_t);
    ~CameraDescriptorSet();

    void update(std::vector<XrView>*);
    void bind(VkCommandBuffer, uint32_t, VkPipelineLayout);

    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

    VulkanBuffer* uniformBuffer = nullptr;
private:
    VulkanInstance* vulkanInstance;
};
