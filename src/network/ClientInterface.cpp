#include "network/ClientInterface.hpp"

#include <sstream>

#include "log/log.hpp"

ClientInterface::ClientInterface(const char* serverAddress, int port)
{
    if(!connect(serverAddress, port)) {
        std::ostringstream err;
        err << "Failed to connect to server ";
        err << serverAddress << ":" << port << ".";
        log_ftl(err.str().c_str());
    }

    if(!authenticate()) {
        std::ostringstream err;
        err << "Failed to authenticate to server ";
        err << serverAddress << ":" << port << ".";
        log_ftl(err.str().c_str());
    }
}
