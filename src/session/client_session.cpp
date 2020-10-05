#include "session/client_session.hpp"

#include <sstream>

#include "log/log.hpp"
#include "network/NetworkInterface.hpp"
#include "xr/XRDisplay.hpp"

bool client_session_run(const char* serverAddress, int port)
{
    XRDisplay xr;

    if(!xr.initialize()) {
        log_err("Failed to initialize XR display.");
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

    log_dbg("Connected to server.");
    log_inf("Verifying...");

    if(!network.verify()) {
        std::ostringstream err;
        err << "Failed to verify on server ";
        err << serverAddress << ":" << port << ".";
        log_err(err.str().c_str());
        return false;
    }

    log_dbg("Verified on server.");

    while(true) {
        if(xr.shouldQuit) break;
    }

    log_dbg("Ending client session.");
    network.disconnect();
    xr.cleanup();

    return true;
}
