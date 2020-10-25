#pragma once

class NetworkShared {
 public:
  ~NetworkShared();

 protected:
  bool connect(const char*, int);
  bool authenticate();

 private:
};
