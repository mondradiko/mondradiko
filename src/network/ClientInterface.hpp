#pragma once

#include <queue>

#include "network/NetworkInterface.hpp"

enum class ClientEventType {
    Ping
};

struct ClientEvent
{
    ClientEventType type;
};

enum class ClientState {
    Disconnected,
    Connecting,
    Authenticating,
    Connected
};

class ClientInterface : public NetworkInterface
{
public:
    ClientInterface(const char*, int);
    ~ClientInterface();

    void update();
    bool readEvent(ClientEvent*);
    void disconnect();

    ClientState state = ClientState::Disconnected;
private:
    std::queue<ClientEvent> eventQueue;
};
