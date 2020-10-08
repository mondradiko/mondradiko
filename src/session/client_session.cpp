#include "session/client_session.hpp"

#include <csignal>
#include <iostream>
#include <sstream>

#include "graphics/Renderer.hpp"
#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "network/NetworkInterface.hpp"
#include "xr/XrDisplay.hpp"
#include "xr/Session.hpp"

bool shouldQuit = false;

void signalHandler(int signum) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";
   shouldQuit = true;

   return;
}

void client_session_run(const char* serverAddress, int port)
{
    XrDisplay xr;
    VulkanInstance vulkanInstance(&xr);
    Session session(&xr, &vulkanInstance);
    Renderer renderer(&vulkanInstance, &session);

    NetworkInterface network;

    if(!network.connect(serverAddress, port)) {
        std::ostringstream err;
        err << "Failed to connect to server ";
        err << serverAddress << ":" << port << ".";
        log_ftl(err.str().c_str());
    }

    if(!network.authenticate()) {
        std::ostringstream err;
        err << "Failed to authenticate to server ";
        err << serverAddress << ":" << port << ".";
        log_ftl(err.str().c_str());
    }

    if(signal(SIGTERM, signalHandler) == SIG_ERR) {
        log_wrn("Can't catch SIGTERM");
    }

    if(signal(SIGINT, signalHandler) == SIG_ERR) {
        log_wrn("Can't catch SIGINT");
    }

    bool shouldRun = false;

    while(true) {
        session.pollEvents(&shouldRun, &shouldQuit);
        if(shouldQuit) break;

        if(shouldRun) {
            double dt;
            bool shouldRender;
            session.beginFrame(&dt, &shouldRender);

            if(shouldRender) {
                renderer.renderFrame();
            }

            session.endFrame(&renderer, shouldRender);
        }
    }
}
