#include "graphics/Renderer.hpp"

#include "log/log.hpp"
#include "xr/XrDisplay.hpp"

Renderer::~Renderer()
{
    log_dbg("Cleaning up renderer.");
    if(instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
}

bool Renderer::initialize(RendererRequirements* requirements)
{
    log_dbg("Initializing renderer.");

    if(!createInstance(requirements)) {
        return false;
    }

    return true;
}

bool Renderer::createInstance(RendererRequirements* requirements)
{
    std::vector<const char*> extensionNames;
    for(uint32_t i = 0; i < requirements->instanceExtensions.size(); i++) {
        extensionNames.push_back(requirements->instanceExtensions[i].c_str());
    }

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Mondradiko",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 0),
        .pEngineName = "Mondradiko",
        .engineVersion = VK_MAKE_VERSION(0, 0, 0),
        .apiVersion = requirements->minApiVersion
    };

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
        .ppEnabledExtensionNames = extensionNames.data()
    };

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        log_err("Failed to create Vulkan instance.");
        return false;
    }

    return true;
}
