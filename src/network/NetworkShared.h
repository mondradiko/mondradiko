/**
 * @file NetworkShared.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Provides shared functionality between client and server derived
 * classes.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_NETWORK_NETWORKSHARED_H_
#define SRC_NETWORK_NETWORKSHARED_H_

namespace mondradiko {

class NetworkShared {
 public:
  ~NetworkShared();

 protected:
  bool connect(const char*, int);
  bool authenticate();

 private:
};

}  // namespace mondradiko

#endif  // SRC_NETWORK_NETWORKSHARED_H_
