#include "network/NetworkInterface.hpp"

#include "log/log.hpp"

NetworkInterface::~NetworkInterface()
{
    log_dbg("Disconnecting from server.");
}

bool NetworkInterface::connect(const char* serverAddress, int port)
{
    log_dbg("Connecting to server.");

    return true;
}

bool NetworkInterface::authenticate()
{
    log_dbg("Authenticating client identity.");

    return true;
}
