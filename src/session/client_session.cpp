#include "session/client_session.hpp"

#include <sstream>

#include "graphics/Renderer.hpp"
#include "log/log.hpp"
#include "network/NetworkInterface.hpp"
#include "xr/XrDisplay.hpp"

bool client_session_run(const char* serverAddress, int port)
{
    XrDisplay xr;

    if(!xr.initialize()) {
        log_err("Failed to initialize XR display.");
        return false;
    }

    Renderer renderer;

    if(!renderer.initialize(&xr)) {
        log_err("Failed to initialize renderer.");
        return false;
    }

    if(!xr.createSession(&renderer)) {
        log_err("Failed to start XR session.");
        return false;
    }

    NetworkInterface network;

    if(!network.connect(serverAddress, port)) {
        std::ostringstream err;
        err << "Failed to connect to server ";
        err << serverAddress << ":" << port << ".";
        log_err(err.str().c_str());
        return false;
    }

    if(!network.authenticate()) {
        std::ostringstream err;
        err << "Failed to authenticate to server ";
        err << serverAddress << ":" << port << ".";
        log_err(err.str().c_str());
        return false;
    }

    double dt;
    bool shouldQuit = false;

    while(!shouldQuit) {
        xr.beginFrame(&dt, &shouldQuit);

        xr.endFrame();
    }

    xr.destroySession();

    return true;
}
