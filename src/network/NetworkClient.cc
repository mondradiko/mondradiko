#include "network/NetworkClient.h"

#include <sstream>

#include "log/log.h"

NetworkClient::NetworkClient(const char* serverAddress, int port) {
  state = ClientState::Connecting;

  if (!connect(serverAddress, port)) {
    disconnect();
    std::ostringstream err;
    err << "Failed to connect to server ";
    err << serverAddress << ":" << port << ".";
    log_ftl(err.str().c_str());
  }

  state = ClientState::Authenticating;

  if (!authenticate()) {
    disconnect();
    std::ostringstream err;
    err << "Failed to authenticate to server ";
    err << serverAddress << ":" << port << ".";
    log_ftl(err.str().c_str());
  }

  state = ClientState::Connected;
}

NetworkClient::~NetworkClient() { disconnect(); }

void NetworkClient::update() {}

bool NetworkClient::readEvent(ClientEvent* event) {
  if (!eventQueue.empty()) {
    *event = eventQueue.front();
    eventQueue.pop();
    return true;
  }

  return false;
}

void NetworkClient::disconnect() { state = ClientState::Disconnected; }
