#include "xr/XrDisplay.hpp"

#include <cstdio>
#include <vector>

#include <vulkan/vulkan.h>

#include "log/log.hpp"
#include "graphics/Renderer.hpp"

XrDisplay::~XrDisplay()
{
    log_dbg("Cleaning up OpenXR.");
    if(session != XR_NULL_HANDLE) xrDestroySession(session);
    if(instance != XR_NULL_HANDLE) xrDestroyInstance(instance);
}

bool XrDisplay::initialize(RendererRequirements* requirements)
{
    log_dbg("Initializing OpenXR.");

    if(!createInstance()) {
        return false;
    }

    if(!findSystem()) {
        return false;
    }

    if(!getRequirements(requirements)) {
        return false;
    }

    shouldQuit = true;

    return true;
}

bool XrDisplay::createSession(Renderer* renderer)
{
    log_dbg("Creating OpenXR session.");

    XrGraphicsBindingVulkanKHR vulkanBindings{
        .type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR
    };

    XrSessionCreateInfo createInfo{
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &vulkanBindings,
        .systemId = systemId
    };

    if(xrCreateSession(instance, &createInfo, &session) != XR_SUCCESS) {
        log_err("Failed to create OpenXR session.");
        return false;
    }

    return true;
}

bool XrDisplay::createInstance()
{
    log_inf("Creating OpenXR instance.");

    XrApplicationInfo appInfo{
        .applicationVersion = XR_MAKE_VERSION(0, 0, 0),
        .engineVersion = XR_MAKE_VERSION(0, 0, 0),
        .apiVersion = XR_MAKE_VERSION(1, 0, 0)
    };

    sprintf(appInfo.applicationName, "Mondradiko Client");
    sprintf(appInfo.engineName, "Mondradiko");

    std::vector<const char*> extensions{
        "XR_KHR_vulkan_enable"
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
        log_err("Failed to create OpenXR instance. Is an OpenXR runtime running?");
        return false;
    }

    xrGetInstanceProcAddr(instance, "xrGetVulkanGraphicsRequirementsKHR", (PFN_xrVoidFunction *)(&ext_xrGetVulkanGraphicsRequirementsKHR));

    return true;
}

bool XrDisplay::findSystem()
{
    log_inf("Choosing OpenXR system.");

    XrSystemGetInfo systemInfo{
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    if(xrGetSystem(instance, &systemInfo, &systemId) != XR_SUCCESS) {
        log_err("Failed to find OpenXR HMD.");
        return false;
    }

    return true;
}

bool XrDisplay::getRequirements(RendererRequirements* requirements)
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

    return true;
}
