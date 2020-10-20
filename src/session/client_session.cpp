#include "session/client_session.hpp"

#include <csignal>
#include <iostream>
#include <sstream>

#include "filesystem/Filesystem.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/VulkanInstance.hpp"
#include "log/log.hpp"
#include "network/ClientInterface.hpp"
#include "scene/Scene.hpp"
#include "xr/PlayerSession.hpp"
#include "xr/XrDisplay.hpp"

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
    PlayerSession session(&xr, &vulkanInstance);
    Renderer renderer(&vulkanInstance, &session);
    ClientInterface client(serverAddress, port);
    Scene scene(&fs, &renderer);

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

            client.update();
            scene.update(dt);

            ClientEvent event;
            while(client.readEvent(&event)) {
                log_dbg("Received client event.");
            }

            if(shouldRender) {
                renderer.renderFrame();
            }

            session.endFrame(&renderer, shouldRender);
        }
    }
}
