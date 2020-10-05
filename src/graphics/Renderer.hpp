#pragma once

#include <vulkan/vulkan.h>

class XrDisplay;
struct RendererRequirements;

class Renderer
{
public:
    ~Renderer();

    bool initialize(XrDisplay*);

    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;

    uint32_t graphicsQueueFamily;
private:
    bool createInstance(RendererRequirements*);
    bool findPhysicalDevice(XrDisplay*);
    bool findQueueFamilies();
    bool createLogicalDevice(RendererRequirements*);
};
