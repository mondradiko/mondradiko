#pragma once

#include <vector>

class Session;
class Viewport;
class VulkanInstance;

class Renderer
{
public:
    Renderer(VulkanInstance*, Session*);
    ~Renderer();

    std::vector<Viewport> viewports;
private:
};
