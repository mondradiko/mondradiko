#pragma once

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
    CameraDescriptorSet(VulkanInstance*);
    ~CameraDescriptorSet();

    void update();
    void bind(VkCommandBuffer, VkPipelineLayout);

    VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VmaAllocation uniformAllocation = nullptr;
    VmaAllocationInfo uniformAllocationInfo;
    VkBuffer uniformBuffer = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;
};
