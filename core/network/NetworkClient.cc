// Copyright (c) 2020-2021 the Mondradiko contributors.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "core/network/NetworkClient.h"

#include <sstream>

#include "core/avatars/Avatar.h"
#include "core/cvars/BoolCVar.h"
#include "core/cvars/StringCVar.h"
#include "core/filesystem/Filesystem.h"
#include "core/ui/UserInterface.h"
#include "core/world/World.h"
#include "log/log.h"
#include "types/protocol/ClientEvent_generated.h"
#include "types/protocol/ServerEvent_generated.h"
#include "types/protocol/WorldEvent_generated.h"

namespace mondradiko {
namespace core {

void NetworkClient::initCVars(CVarScope* cvars) {
  CVarScope* client = cvars->addChild("client");

  client->addValue<StringCVar>("username");
  client->addValue<StringCVar>("metaverse_provider");
}

// Nasty singleton
// GameNetworkingSockets doesn't give us a choice here
NetworkClient* g_client = nullptr;

NetworkClient::NetworkClient(const CVarScope* cvars, Filesystem* fs,
                             UserInterface* ui, World* world)
    : cvars(cvars->getChild("client")), fs(fs), ui(ui), world(world) {
  log_zone;

  if (g_client) {
    log_ftl("NetworkClient already exists");
  }

  g_client = this;

  SteamDatagramErrMsg err;
  if (!GameNetworkingSockets_Init(nullptr, err)) {
    types::string message = err;
    log_ftl_fmt("Failed to initialize SteamNetworkingSockets with error: %s",
                message.c_str());
  }

  sockets = SteamNetworkingSockets();
}

NetworkClient::~NetworkClient() {
  log_zone;

  disconnect();
}

bool NetworkClient::connect(const Avatar* client_avatar, const char* server_ip,
                            int server_port) {
  avatar = client_avatar;

  SteamNetworkingIPAddr server_addr;
  server_addr.Clear();

  if (!server_addr.ParseString(server_ip)) {
    log_err_fmt("Failed to parse server address %s", server_ip);
    return false;
  }

  server_addr.m_port = server_port;

  char szAddr[SteamNetworkingIPAddr::k_cchMaxString];
  server_addr.ToString(szAddr, sizeof(szAddr), true);
  types::string addr_string = szAddr;
  log_msg_fmt("Connecting to server at %s", addr_string.c_str());

  SteamNetworkingConfigValue_t callback;
  callback.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged,
                  reinterpret_cast<void*>(callback_ConnectionStatusChanged));
  connection = sockets->ConnectByIPAddress(server_addr, 1, &callback);

  if (connection == k_HSteamNetConnection_Invalid) {
    log_err("Failed to create connection");
    return false;
  }

  return true;
}

void NetworkClient::update() {
  log_zone;

  sockets->RunCallbacks();

  if (state == ClientState::Disconnected) return;

  receiveEvents();

  updateAvatar();

  if (event_queue.size() > 0) {
    sendQueuedEvents();
  }
}

void NetworkClient::disconnect() {
  log_zone;

  if (connection != k_HSteamNetConnection_Invalid) {
    sockets->CloseConnection(connection, 0, "Client disconnect", true);
    connection = k_HSteamNetConnection_Invalid;
  }

  state = ClientState::Disconnected;
}

///////////////////////////////////////////////////////////////////////////////
// Server event receive callbacks
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::onAnnouncement(const protocol::Announcement* announcement) {
  log_msg_fmt("Server announcement: %s", announcement->message()->c_str());
  ui->displayMessage(announcement->message()->c_str());
}

void NetworkClient::onAssignClientId(
    const protocol::AssignClientId* assign_client_id) {
  client_id = static_cast<ClientId>(assign_client_id->new_id());
  log_msg_fmt("Assigned new ID #%d", client_id);
  state = ClientState::Joined;
}

///////////////////////////////////////////////////////////////////////////////
// Client event send methods
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::requestJoin() {
  flatbuffers::FlatBufferBuilder builder;
  builder.Clear();

  protocol::AvatarType avatar_type = protocol::AvatarType::None;
  if (avatar != nullptr) {
    avatar_type = avatar->getProtocolType();
  }

  auto username_offset =
      builder.CreateString(cvars->get<StringCVar>("username").str());

  // TODO(marceline-cramer) Use strings for hashes
  types::vector<uint64_t> lump_checksums;

  {
    types::vector<assets::LumpHash> local_checksums;
    fs->getChecksums(local_checksums);
    // Convert from internal LumpHash type to protocol uint64
    for (auto& checksum : local_checksums) {
      lump_checksums.push_back(static_cast<uint64_t>(checksum));
    }
  }

  auto lump_checksums_offset = builder.CreateVector(lump_checksums);

  protocol::JoinRequestBuilder join_request(builder);
  join_request.add_username(username_offset);
  join_request.add_avatar_type(avatar_type);
  join_request.add_lump_checksums(lump_checksums_offset);
  auto join_request_offset = join_request.Finish();

  protocol::ClientEventBuilder event(builder);
  event.add_type(protocol::ClientEventType::JoinRequest);
  event.add_join_request(join_request_offset);
  auto event_offset = event.Finish();

  builder.Finish(event_offset);
  sendEvent(builder);
}

void NetworkClient::updateAvatar() {
  if (avatar == nullptr) {
    return;
  }

  flatbuffers::FlatBufferBuilder fbb;
  fbb.Clear();

  auto data_offset = avatar->serialize(&fbb);

  protocol::AvatarUpdateBuilder avatar_update(fbb);
  avatar_update.add_data(data_offset);
  auto avatar_update_offset = avatar_update.Finish();

  protocol::ClientEventBuilder event(fbb);
  event.add_type(protocol::ClientEventType::AvatarUpdate);
  event.add_avatar_update(avatar_update_offset);
  auto event_offset = event.Finish();

  fbb.Finish(event_offset);
  sendEvent(fbb);
}

///////////////////////////////////////////////////////////////////////////////
// Connection status change callbacks
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::onConnecting(HSteamNetConnection) {
  log_msg("Connecting");
  state = ClientState::Connecting;
}

void NetworkClient::onConnected(HSteamNetConnection) {
  log_msg("Connected");
  requestJoin();
}

void NetworkClient::onProblemDetected(HSteamNetConnection) {
  log_err("Failed to reach remote host");
}

void NetworkClient::onClosedByPeer(HSteamNetConnection) {
  log_err("Problem detected locally");
}

void NetworkClient::onDisconnect(HSteamNetConnection connection) {
  log_err("Forcefully disconnected");
  sockets->CloseConnection(connection, 0, nullptr, false);
  connection = k_HSteamNetConnection_Invalid;
  state = ClientState::Disconnected;
}

///////////////////////////////////////////////////////////////////////////////
// Helper methods
///////////////////////////////////////////////////////////////////////////////

void NetworkClient::receiveEvents() {
  while (true) {
    ISteamNetworkingMessage* incoming_msg = nullptr;
    int msg_num =
        sockets->ReceiveMessagesOnConnection(connection, &incoming_msg, 1);

    if (msg_num == 0) break;

    if (msg_num < 0) {
      log_err("Error receiving messages");
      break;
    }

    auto event = protocol::GetServerEvent(incoming_msg->GetData());

    switch (event->type()) {
      case protocol::ServerEventType::NoMessage: {
        log_wrn("Received empty server event");
        break;
      }

      case protocol::ServerEventType::WorldUpdate: {
        auto world_update = event->world_update();
        for (uint32_t i = 0; i < world_update->size(); i++) {
          world->processEvent(world_update->Get(i));
        }

        break;
      }

      case protocol::ServerEventType::Announcement: {
        onAnnouncement(event->announcement());
        break;
      }

      case protocol::ServerEventType::AssignClientId: {
        onAssignClientId(event->assign_client_id());
        break;
      }

      default: {
        log_err_fmt("Unhandled server event %hu", event->type());
        break;
      }
    }  // switch (event->type())

    incoming_msg->Release();
  }
}

void NetworkClient::sendEvent(flatbuffers::FlatBufferBuilder& builder) {
  uint8_t* event_data = builder.GetBufferPointer();
  size_t event_size = builder.GetSize();
  auto& event_buffer = event_queue.emplace_back();
  event_buffer.data.resize(event_size);
  memcpy(event_buffer.data.data(), event_data, event_size);
}

void NetworkClient::sendQueuedEvents() {
  types::vector<SteamNetworkingMessage_t*> outgoing_messages;
  outgoing_messages.reserve(event_queue.size());

  for (uint32_t i = 0; i < event_queue.size(); i++) {
    auto& event = event_queue[i];

    // TODO(marceline-cramer) Avoid copies
    size_t message_size = event.data.size();
    SteamNetworkingMessage_t* message =
        SteamNetworkingUtils()->AllocateMessage(message_size);
    memcpy(message->m_pData, event.data.data(), message_size);

    message->m_conn = connection;
    message->m_nFlags = k_nSteamNetworkingSend_Reliable;

    outgoing_messages.push_back(message);
  }

  sockets->SendMessages(outgoing_messages.size(), outgoing_messages.data(),
                        nullptr);

  event_queue.clear();
}

void NetworkClient::callback_ConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* event) {
  log_zone;

  types::string description = event->m_info.m_szConnectionDescription;

  switch (event->m_info.m_eState) {
    case k_ESteamNetworkingConnectionState_Connecting: {
      g_client->onConnecting(event->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_Connected: {
      g_client->onConnected(event->m_hConn);
      break;
    }

    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    case k_ESteamNetworkingConnectionState_ClosedByPeer: {
      if (event->m_eOldState ==
          k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
        g_client->onProblemDetected(event->m_hConn);
      } else if (event->m_eOldState ==
                 k_ESteamNetworkingConnectionState_Connecting) {
        g_client->onProblemDetected(event->m_hConn);
      } else {
        g_client->onClosedByPeer(event->m_hConn);
      }

      g_client->onDisconnect(event->m_hConn);

      break;
    }

    case k_ESteamNetworkingConnectionState_None: {
      // Called when connections are destroyed
      break;
    }

    default: {
      log_wrn_fmt("Uncaught connection status change %s", description.c_str());
      break;
    }
  }
}

}  // namespace core
}  // namespace mondradiko
