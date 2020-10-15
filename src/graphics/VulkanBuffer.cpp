#include "graphics/VulkanBuffer.hpp"

#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"

VulkanBuffer::VulkanBuffer(VulkanInstance* vulkanInstance, size_t bufferSize, VkBufferUsageFlags bufferUsageFlags, VmaMemoryUsage memoryUsage)
 : bufferSize(bufferSize),
   vulkanInstance(vulkanInstance)
{
    VkBufferCreateInfo vertexBufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferSize,
        .usage = bufferUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo allocationCreateInfo{
        .usage = memoryUsage
    };

    if(vmaCreateBuffer(vulkanInstance->allocator, &vertexBufferInfo, &allocationCreateInfo, &buffer, &allocation, &allocationInfo) != VK_SUCCESS) {
        log_ftl("Failed to allocate Vulkan buffer.");
    }
}

VulkanBuffer::~VulkanBuffer()
{
    if(allocation != nullptr) vmaDestroyBuffer(vulkanInstance->allocator, buffer, allocation);
}

void VulkanBuffer::writeData(void* src)
{
    void* dst;
    vkMapMemory(vulkanInstance->device, allocationInfo.deviceMemory, allocationInfo.offset, bufferSize, 0, &dst);
        memcpy(dst, src, bufferSize);
    vkUnmapMemory(vulkanInstance->device, allocationInfo.deviceMemory);
}
