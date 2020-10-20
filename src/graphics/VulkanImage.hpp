#pragma once

#include "api_headers.hpp"

class VulkanInstance;

class VulkanImage
{
public:
    VulkanImage(VulkanInstance*, VkFormat, uint32_t, uint32_t, VkImageUsageFlags, VmaMemoryUsage);
    ~VulkanImage();

    VkFormat format;
    uint32_t width;
    uint32_t height;
    VmaAllocation allocation = nullptr;
    VmaAllocationInfo allocationInfo;
    VkImage image = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;
};
