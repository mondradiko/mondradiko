#include "xr/XrDisplay.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "build_config.h"
#include "log/log.hpp"
#include "graphics/VulkanInstance.hpp"
#include "graphics/Viewport.hpp"

static XRAPI_ATTR XrBool32 XRAPI_CALL debugCallback(
    XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
    XrDebugUtilsMessageTypeFlagsEXT messageType,
    const XrDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    
    Log::LogLevel severity;

    switch(messageSeverity) {
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        severity = Log::LOG_LEVEL_INFO;
        break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        severity = Log::LOG_LEVEL_WARNING;
        break;
    case XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
        severity = Log::LOG_LEVEL_ERROR;
        break;
    }

    Log::log("OpenXRValidation", 0, severity, pCallbackData->message);
    return VK_FALSE;
}

XrDisplay::XrDisplay()
{
    createInstance();

    if(enableValidationLayers) {
        setupDebugMessenger();
    }

    findSystem();
}

XrDisplay::~XrDisplay()
{
    log_dbg("Cleaning up XrDisplay.");

    if(enableValidationLayers && debugMessenger != VK_NULL_HANDLE)
        ext_xrDestroyDebugUtilsMessengerEXT(debugMessenger);

    if(instance != XR_NULL_HANDLE) xrDestroyInstance(instance);
}

bool XrDisplay::getRequirements(VulkanRequirements* requirements)
{
    XrGraphicsRequirementsVulkanKHR vulkanRequirements{
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR
    };

    if(ext_xrGetVulkanGraphicsRequirementsKHR(instance, systemId, &vulkanRequirements) != XR_SUCCESS) {
        log_err("Failed to get OpenXR Vulkan requirements.");
        return false;
    }

    requirements->minApiVersion = vulkanRequirements.minApiVersionSupported;
    requirements->maxApiVersion = vulkanRequirements.maxApiVersionSupported;

    uint32_t instanceExtensionsLength;
    ext_xrGetVulkanInstanceExtensionsKHR(instance, systemId, 0, &instanceExtensionsLength, nullptr);
    std::vector<char> instanceExtensionsList(instanceExtensionsLength);
    ext_xrGetVulkanInstanceExtensionsKHR(instance, systemId, instanceExtensionsLength, &instanceExtensionsLength, instanceExtensionsList.data());
    std::string instanceExtensionNames = instanceExtensionsList.data();

    // The list of required instance extensions is separated by spaces, so parse it out
    log_inf("Vulkan instance extensions required by OpenXR:");
    requirements->instanceExtensions.clear();
    uint32_t startIndex = 0;
    for(uint32_t endIndex = 0; endIndex <= instanceExtensionNames.size(); endIndex++) {
        if(instanceExtensionNames[endIndex] == ' ' || instanceExtensionNames[endIndex] == '\0') {
            std::string extension = instanceExtensionNames.substr(startIndex, endIndex-startIndex);
            log_inf(extension.c_str());
            requirements->instanceExtensions.push_back(extension);
            startIndex = endIndex + 1;
        }
    }

    uint32_t deviceExtensionsLength;
    ext_xrGetVulkanDeviceExtensionsKHR(instance, systemId, 0, &deviceExtensionsLength, nullptr);
    std::vector<char> deviceExtensionsList(deviceExtensionsLength);
    ext_xrGetVulkanDeviceExtensionsKHR(instance, systemId, deviceExtensionsLength, &deviceExtensionsLength, deviceExtensionsList.data());
    std::string deviceExtensionNames = deviceExtensionsList.data();

    // The list of required device extensions is separated by spaces, so parse it out
    log_inf("Vulkan device extensions required by OpenXR:");
    requirements->deviceExtensions.clear();
    startIndex = 0;
    for(uint32_t endIndex = 0; endIndex <= deviceExtensionNames.size(); endIndex++) {
        if(deviceExtensionNames[endIndex] == ' ' || deviceExtensionNames[endIndex] == '\0') {
            std::string extension = deviceExtensionNames.substr(startIndex, endIndex-startIndex);
            log_inf(extension.c_str());
            requirements->deviceExtensions.push_back(extension);
            startIndex = endIndex + 1;
        }
    }

    return true;
}

bool XrDisplay::getVulkanDevice(VkInstance vkInstance, VkPhysicalDevice* vkPhysicalDevice)
{
    if(ext_xrGetVulkanGraphicsDeviceKHR(instance, systemId, vkInstance, vkPhysicalDevice) != XR_SUCCESS) {
        log_err("Failed to get Vulkan physical device.");
        return false;
    }

    return true;
}

void XrDisplay::populateDebugMessengerCreateInfo(XrDebugUtilsMessengerCreateInfoEXT* createInfo)
{
    *createInfo = {
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                             | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                             | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                        | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                        | XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                        | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT,
        .userCallback = debugCallback
    };
}

void XrDisplay::createInstance()
{
    log_dbg("Creating OpenXR instance.");

    XrApplicationInfo appInfo{
        .applicationVersion = XR_MAKE_VERSION(0, 0, 0),
        .engineVersion = MONDRADIKO_OPENXR_VERSION,
        .apiVersion = XR_MAKE_VERSION(1, 0, 0)
    };

    sprintf(appInfo.applicationName, "Mondradiko Client");
    sprintf(appInfo.engineName, MONDRADIKO_NAME);

    std::vector<const char*> extensions{
        XR_KHR_VULKAN_ENABLE_EXTENSION_NAME,
        XR_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    // TODO Validation layers
    XrInstanceCreateInfo createInfo{
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .applicationInfo = appInfo,
        .enabledApiLayerCount = 0,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .enabledExtensionNames = extensions.data()
    };

    XrResult result = xrCreateInstance(&createInfo, &instance);

    if(result != XR_SUCCESS || instance == nullptr) {
        log_ftl("Failed to create OpenXR instance. Is an OpenXR runtime running?");
    }

    xrGetInstanceProcAddr(instance, "xrCreateDebugUtilsMessengerEXT", (PFN_xrVoidFunction *)(&ext_xrCreateDebugUtilsMessengerEXT));
    xrGetInstanceProcAddr(instance, "xrDestroyDebugUtilsMessengerEXT", (PFN_xrVoidFunction *)(&ext_xrDestroyDebugUtilsMessengerEXT));
    xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction *)(&ext_xrGetVulkanGraphicsRequirementsKHR));
    xrGetInstanceProcAddr(instance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction *)(&ext_xrGetVulkanInstanceExtensionsKHR));
    xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction *)(&ext_xrGetVulkanGraphicsDeviceKHR));
    xrGetInstanceProcAddr(instance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction *)(&ext_xrGetVulkanDeviceExtensionsKHR));
}

void XrDisplay::setupDebugMessenger()
{
    XrDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);

    if(ext_xrCreateDebugUtilsMessengerEXT(instance, &createInfo, &debugMessenger) != XR_SUCCESS) {
        log_ftl("Failed to create OpenXR debug messenger.");
    }
}

void XrDisplay::findSystem()
{
    log_dbg("Choosing OpenXR system.");

    XrSystemGetInfo systemInfo{
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    if(xrGetSystem(instance, &systemInfo, &systemId) != XR_SUCCESS) {
        log_ftl("Failed to find OpenXR HMD.");
    }
}
