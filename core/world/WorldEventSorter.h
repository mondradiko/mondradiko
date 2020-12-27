/**
 * @file WorldEventSorter.h
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Determines which WorldEvents go to which clients.
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#pragma once

#include <deque>
#include <memory>

#include "flatbuffers/flatbuffers.h"

namespace mondradiko {

// Forward declarations
struct World;

namespace protocol {
struct WorldEvent;
struct WorldEventT;
}  // namespace protocol

class WorldEventSorter {
 public:
  explicit WorldEventSorter(World*);
  ~WorldEventSorter();

  void processEvent(std::unique_ptr<protocol::WorldEventT>&);

  using WorldUpdate = flatbuffers::Vector<
      flatbuffers::Offset<mondradiko::protocol::WorldEvent>>;
  using WorldUpdateOffset = flatbuffers::Offset<WorldUpdate>;

  WorldUpdateOffset broadcastGlobalEvents(
      flatbuffers::FlatBufferBuilder&) const;

  bool isOutOfDate();
  void clearQueue();

 private:
  World* world;

  using WorldEventQueue = std::deque<std::shared_ptr<protocol::WorldEventT>>;

  WorldEventQueue global_events;
};

}  // namespace mondradiko
