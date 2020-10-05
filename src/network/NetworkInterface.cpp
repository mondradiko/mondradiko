#include "network/NetworkInterface.hpp"

#include "log/log.hpp"

NetworkInterface::~NetworkInterface()
{
    log_dbg("Disconnecting from server.");
}

bool NetworkInterface::connect(const char* serverAddress, int port)
{
    return true;
}

bool NetworkInterface::verify()
{
    return true;
}
