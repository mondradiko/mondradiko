#include "session/client_session.h"

#include <csignal>
#include <iostream>
#include <sstream>

#include "filesystem/Filesystem.h"
#include "graphics/Renderer.h"
#include "graphics/VulkanInstance.h"
#include "log/log.h"
#include "network/ClientInterface.h"
#include "scene/Scene.h"
#include "xr/PlayerSession.h"
#include "xr/XrDisplay.h"

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
