#pragma once

#include <vector>

#include "api_headers.hpp"

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

    void update(uint32_t);
    void bind(VkCommandBuffer, uint32_t, VkPipelineLayout);

    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    VmaAllocation uniformAllocation = nullptr;
    VmaAllocationInfo uniformAllocationInfo;
    VkBuffer uniformBuffer = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;
};
