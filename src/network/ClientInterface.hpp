#pragma once

#include "network/NetworkInterface.hpp"

class ClientInterface : public NetworkInterface
{
public:
    ClientInterface(const char*, int);
private:
};
