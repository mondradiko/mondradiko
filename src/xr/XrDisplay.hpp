#pragma once

#include <openxr/openxr.h>

class XrDisplay
{
public:
    bool initialize();
    void cleanup();

    bool shouldQuit = false;
private:
    bool createInstance();
    
    XrInstance instance;
    XrSession session;
};
