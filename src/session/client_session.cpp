#include "session/client_session.hpp"

#include <csignal>
#include <iostream>
#include <sstream>

#include "graphics/Renderer.hpp"
#include "log/log.hpp"
#include "network/NetworkInterface.hpp"
#include "xr/XrDisplay.hpp"

bool shouldQuit = false;

void signalHandler(int signum) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";
   shouldQuit = true;

   return;
}

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

    if(!network.authenticate()) {
        std::ostringstream err;
        err << "Failed to authenticate to server ";
        err << serverAddress << ":" << port << ".";
        log_err(err.str().c_str());
        return false;
    }

    if(signal(SIGTERM, signalHandler) == SIG_ERR) {
        log_wrn("Can't catch SIGTERM");
    }

    if(signal(SIGINT, signalHandler) == SIG_ERR) {
        log_wrn("Can't catch SIGINT");
    }

    bool shouldRun = false;

    while(true) {
        xr.pollEvents(&shouldRun, &shouldQuit);
        if(shouldQuit) break;

        if(shouldRun) {
            double dt;
            bool shouldRender;
            xr.beginFrame(&dt, &shouldRender);

            if(shouldRender) {
                log_dbg("Rendering scene.");
            }

            xr.endFrame();
        }
    }

    xr.destroySession();

    return true;
}
