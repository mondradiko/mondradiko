#pragma once

class NetworkInterface
{
public:
    ~NetworkInterface();
protected:
    bool connect(const char*, int);
    bool authenticate();
private:
};
