#pragma once

class XRDisplay
{
public:
    bool initialize();
    void cleanup();

    bool shouldQuit = false;
private:
};
