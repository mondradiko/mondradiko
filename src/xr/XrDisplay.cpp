#include "xr/XrDisplay.hpp"

#include <cstdio>

#include "log/log.hpp"

bool XrDisplay::initialize()
{
    log_dbg("Initializing OpenXR.");

    if(!createInstance()) {
        return false;
    }

    shouldQuit = true;
    return true;
}

void XrDisplay::cleanup()
{
    log_dbg("Cleaning up OpenXR.");
    xrDestroyInstance(instance);
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

    XrInstanceCreateInfo createInfo{
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .applicationInfo = appInfo,
        .enabledApiLayerCount = 0,
        .enabledExtensionCount = 0
    };

    XrResult result = xrCreateInstance(&createInfo, &instance);

    if(result != XR_SUCCESS || instance == nullptr) {
        log_err("Failed to create OpenXR instance. Is an OpenXR runtime running?");
        return false;
    }

    return true;
}
