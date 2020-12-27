/**
 * @file WorldEventSorter.cc
 * @author Marceline Cramer (cramermarceline@gmail.com)
 * @brief Determines which WorldEvents go to which clients.
 * @date 2020-12-23
 *
 * @copyright Copyright (c) 2020 Marceline Cramer
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 */

#include "core/world/WorldEventSorter.h"

#include <utility>
#include <vector>

#include "core/world/World.h"
#include "protocol/WorldEvent_generated.h"

namespace mondradiko {

WorldEventSorter::WorldEventSorter(World* world) : world(world) {}

WorldEventSorter::~WorldEventSorter() {}

void WorldEventSorter::processEvent(
    std::unique_ptr<protocol::WorldEventT>& event) {
  global_events.push_back(std::move(event));
}

WorldEventSorter::WorldUpdateOffset WorldEventSorter::broadcastGlobalEvents(
    flatbuffers::FlatBufferBuilder& builder) const {
  std::vector<flatbuffers::Offset<protocol::WorldEvent>> update_offsets;

  for (auto& event : global_events) {
    auto event_offset = protocol::CreateWorldEvent(builder, event.get());
    update_offsets.push_back(event_offset);
  }

  return builder.CreateVector(update_offsets);
}

bool WorldEventSorter::isOutOfDate() { return global_events.size() > 0; }

void WorldEventSorter::clearQueue() { global_events.clear(); }

}  // namespace mondradiko
