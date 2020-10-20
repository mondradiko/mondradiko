#include "graphics/VulkanImage.h"

#include "graphics/VulkanInstance.h"
#include "log/log.h"

VulkanImage::VulkanImage(VulkanInstance* vulkanInstance, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags imageUsage, VmaMemoryUsage memoryUsage)
: format(format),
  width(height),
  height(height)
{
    VkImageCreateInfo imageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = imageUsage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocationCreateInfo{
        .usage = memoryUsage
    };

    if(vmaCreateImage(vulkanInstance->allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation, &allocationInfo) != VK_SUCCESS) {
        log_ftl("Failed to allocate Vulkan image.");        
    }
}

VulkanImage::~VulkanImage()
{
    if(allocation != nullptr) vmaDestroyImage(vulkanInstance->allocator, image, allocation);
}
