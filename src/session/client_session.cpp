#include "session/client_session.hpp"

#include <csignal>
#include <iostream>
#include <sstream>

#include "filesystem/Filesystem.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "network/ClientInterface.hpp"
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
    Filesystem fs("../test-folder/");
    XrDisplay xr;
    VulkanInstance vulkanInstance(&xr);
    Session session(&xr, &vulkanInstance);
    Renderer renderer(&vulkanInstance, &session);
    ClientInterface client(serverAddress, port);

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
