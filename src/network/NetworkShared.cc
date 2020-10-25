#include "network/NetworkShared.h"

#include "log/log.h"

NetworkShared::~NetworkShared() { log_dbg("Disconnecting from server."); }

bool NetworkShared::connect(const char* serverAddress, int port) {
  log_dbg("Connecting to server.");

  return true;
}

bool NetworkShared::authenticate() {
  log_dbg("Authenticating client identity.");

  return true;
}
