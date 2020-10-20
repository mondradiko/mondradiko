#pragma once

#include "api_headers.h"

class VulkanInstance;

class VulkanBuffer
{
public:
    VulkanBuffer(VulkanInstance*, size_t, VkBufferUsageFlags, VmaMemoryUsage);
    ~VulkanBuffer();

    void writeData(void*);

    size_t bufferSize = 0;
    VmaAllocation allocation = nullptr;
    VmaAllocationInfo allocationInfo;
    VkBuffer buffer = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;
};
