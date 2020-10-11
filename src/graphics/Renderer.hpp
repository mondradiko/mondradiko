#pragma once

#include <vector>

#include "api_headers.hpp"

#include "graphics/pipelines/MeshPipeline.hpp"

class Session;
class Viewport;
class VulkanInstance;

struct CameraUniform
{
    glm::mat4 view;
    glm::mat4 projection;
};

class Renderer
{
public:
    Renderer(VulkanInstance*, Session*);
    ~Renderer();

    void renderFrame();
    void finishRender(std::vector<XrView>*, std::vector<XrCompositionLayerProjectionView>*);

    VkFormat swapchainFormat;
    VkRenderPass compositePass = VK_NULL_HANDLE;
    std::vector<Viewport*> viewports;

    MeshPipeline* meshPipeline = nullptr;

    VkDescriptorSetLayout cameraSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet cameraSet = VK_NULL_HANDLE;
    VmaAllocation cameraAllocation = nullptr;
    VmaAllocationInfo cameraAllocationInfo;
    VkBuffer cameraBuffer = VK_NULL_HANDLE;
private:
    VulkanInstance* vulkanInstance;
    Session* session;

    void findSwapchainFormat();
    void createRenderPasses();
    void createCameraDescriptor();
    void createPipelines();
};
