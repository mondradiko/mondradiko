#include "graphics/Renderer.hpp"

#include <cstring>
#include <set>

#include "log/log.hpp"
#include "xr/XrDisplay.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    
    Log::LogLevel severity;

    switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        severity = Log::LOG_LEVEL_INFO;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        severity = Log::LOG_LEVEL_WARNING;
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
        severity = Log::LOG_LEVEL_ERROR;
        break;
    }

    Log::log("Vulkan Validation Layer", 0, severity, pCallbackData->pMessage);
    return VK_FALSE;
}

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

    if(enableValidationLayers) {
        if(!checkValidationLayerSupport()) {
            log_wrn("Vulkan validation layers requested, but not available.");
        }
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

bool Renderer::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers) {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers) {
            if(strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if(!layerFound) {
            return false;
        }
    }

    return true;
}

void Renderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
{
    *createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback
    };
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

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if(enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }

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

    extensionNames.push_back("VK_EXT_debug_utils");

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
