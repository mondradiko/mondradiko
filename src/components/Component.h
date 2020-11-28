/**
 * @file Component.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements reference counting of Assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#ifndef SRC_COMPONENTS_COMPONENT_H_
#define SRC_COMPONENTS_COMPONENT_H_

namespace mondradiko {

class Entity;  // Forward declaration because of codependence

class Component {
 public:
  virtual ~Component();

 private:
  friend class Entity;

  Entity* parent = nullptr;
  Component* prev = nullptr;
  Component* next = nullptr;
};

}  // namespace mondradiko

#endif  // SRC_COMPONENTS_COMPONENT_H_
