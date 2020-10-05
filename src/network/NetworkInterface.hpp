#pragma once

class NetworkInterface
{
public:
    ~NetworkInterface();

    bool connect(const char*, int);
    bool authenticate();
private:
};
