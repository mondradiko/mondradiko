#include "graphics/Renderer.hpp"

#include <set>

#include "log/log.hpp"
#include "xr/XrDisplay.hpp"

Renderer::~Renderer()
{
    log_dbg("Cleaning up renderer.");
    if(device != VK_NULL_HANDLE) vkDestroyDevice(device, nullptr);
    if(instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
}

bool Renderer::initialize(XrDisplay* display)
{
    log_dbg("Initializing renderer.");

    RendererRequirements requirements;

    if(!display->getRequirements(&requirements)) {
        return false;
    }

    if(!createInstance(&requirements)) {
        return false;
    }

    if(!findPhysicalDevice(display)) {
        return false;
    }

    if(!findQueueFamilies()) {
        return false;
    }

    if(!createLogicalDevice(&requirements)) {
        return false;
    }

    return true;
}

bool Renderer::createInstance(RendererRequirements* requirements)
{
    log_inf("Creating Vulkan instance.");

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

bool Renderer::findPhysicalDevice(XrDisplay* display)
{
    log_inf("Finding Vulkan physical device.");

    if(!display->getVulkanDevice(instance, &physicalDevice)) {
        return false;
    }

    return true;
}

bool Renderer::findQueueFamilies()
{
    log_inf("Finding Vulkan queue families.");

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for(uint32_t i=0; i<queueFamilyCount; i++) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamily = i;
            break;
        }
    }

    return true;
}

bool Renderer::createLogicalDevice(RendererRequirements* requirements)
{
    log_inf("Creating Vulkan logical device.");

    std::vector<const char*> extensionNames;
    for(uint32_t i = 0; i < requirements->deviceExtensions.size(); i++) {
        extensionNames.push_back(requirements->deviceExtensions[i].c_str());
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {graphicsQueueFamily};

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies) {
        queueCreateInfos.push_back({
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamily,
            .queueCount = 1,
            .pQueuePriorities = &queuePriority
        });
    }

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .runtimeDescriptorArray = VK_TRUE
    };

    VkPhysicalDeviceFeatures deviceFeatures{
        .multiViewport = VK_TRUE,
        .samplerAnisotropy = VK_TRUE
    };

    VkDeviceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &indexingFeatures,
        .queueCreateInfoCount = (uint32_t) queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
        .ppEnabledExtensionNames = extensionNames.data(),
        .pEnabledFeatures = &deviceFeatures
    };

    if(!vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        log_err("Failed to create Vulkan logical device.");
        return false;
    }

    return true;
}
