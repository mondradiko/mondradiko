#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

class Session;
class Viewport;
class VulkanInstance;

class Renderer
{
public:
    Renderer(VulkanInstance*, Session*);
    ~Renderer();

    VkFormat swapchainFormat;
    VkRenderPass compositePass = VK_NULL_HANDLE;
    std::vector<Viewport> viewports;
private:
    VulkanInstance* vulkanInstance;
    Session* session;

    void findSwapchainFormat();
    void createRenderPasses();
};
