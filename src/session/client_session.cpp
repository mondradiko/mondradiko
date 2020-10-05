#include "session/client_session.hpp"

#include <sstream>

#include "graphics/Renderer.hpp"
#include "log/log.hpp"
#include "network/NetworkInterface.hpp"
#include "xr/XrDisplay.hpp"

bool client_session_run(const char* serverAddress, int port)
{
    Renderer renderer;

    if(!renderer.initialize()) {
        log_err("Failed to initialize renderer.");
        return false;
    }

    XrDisplay xr;

    if(!xr.initialize()) {
        log_err("Failed to initialize XR display.");
        return false;
    }

    if(!xr.createSession(&renderer)) {
        log_err("Failed to create XR session.");
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

    if(!network.verify()) {
        std::ostringstream err;
        err << "Failed to verify on server ";
        err << serverAddress << ":" << port << ".";
        log_err(err.str().c_str());
        return false;
    }

    while(true) {
        if(xr.shouldQuit) break;
    }

    return true;
}
