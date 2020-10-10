#pragma once

#include <vector>

#include "api_headers.hpp"

#include "graphics/pipelines/MeshPipeline.hpp"

class Session;
class Viewport;
class VulkanInstance;

class Renderer
{
public:
    Renderer(VulkanInstance*, Session*);
    ~Renderer();

    void renderFrame();
    void finishRender(std::vector<XrView>*, std::vector<XrCompositionLayerProjectionView>*);

    VkFormat swapchainFormat;
    VkRenderPass compositePass = VK_NULL_HANDLE;
    std::vector<Viewport> viewports;

    MeshPipeline meshPipeline;
private:
    VulkanInstance* vulkanInstance;
    Session* session;

    void findSwapchainFormat();
    void createRenderPasses();
    void initializePipelines();
};
