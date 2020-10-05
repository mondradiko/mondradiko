#pragma once

class NetworkInterface
{
public:
    bool connect(const char*, int);
    bool verify();
    void disconnect();
private:
};
