#pragma once

#include <queue>

#include "network/NetworkShared.h"

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

class NetworkClient : public NetworkShared {
 public:
  NetworkClient(const char*, int);
  ~NetworkClient();

  void update();
  bool readEvent(ClientEvent*);
  void disconnect();

  ClientState state = ClientState::Disconnected;

 private:
  std::queue<ClientEvent> eventQueue;
};
