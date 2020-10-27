/**
 * @file Component.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Implements reference counting of Assets.
 * @date 2020-10-24
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 *
 * This file is part of Mondradiko.
 *
 * Mondradiko is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mondradiko is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Mondradiko.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_COMPONENTS_COMPONENT_H_
#define SRC_COMPONENTS_COMPONENT_H_

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

#endif  // SRC_COMPONENTS_COMPONENT_H_
