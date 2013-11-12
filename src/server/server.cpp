/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#include "server.h"

#include "src/network/packet.h"

#include "src/packet.pb.h"
#include "src/player.h"

#include "src/client/client.h"

#include "src/debug.h"
#include "src/camera.h"
#include "src/world.h"
#include "src/chunk.h"
#include "src/time.h"
#include "src/torch.h"
#include "src/quickbarinventory.h"
#include "src/globals.h"
#include "src/tool.h"
#include "src/settings/settings.h"
#include "src/../config.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <chrono>

#include <algorithm>
#include <QThread>

Server::Server()
{

}

Server::~Server()
{
    enet_host_destroy(m_server);
}

void Server::init(uint8_t maxClients, uint16_t port, Client* client)
{
    m_client = client;

    Debug::log(Debug::Area::NetworkServerInitialArea) << "creating server at port: " << port;

    m_address.host = ENET_HOST_ANY;
    m_address.port = port;

    m_server = enet_host_create(&m_address, maxClients /* allow up to maxClients and/or outgoing connections */,
                                2 /* allow up to 2 channels to be used, 0 and 1 */,
                                0 /* assume any amount of incoming bandwidth */,
                                0 /* assume any amount of outgoing bandwidth */);

   Debug::assertf(m_server, "failed to create ENet server");

    m_server->duplicatePeers = 4;
    enet_host_compress_with_range_coder(m_server);

    m_world = new World(nullptr, nullptr, this);
}

void Server::tick()
{
    std::chrono::system_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

    double accumulator = 0.0;
    const double dt = (FIXED_TIMESTEP) * 1000.0; // runs at 30 hz
    double t = 0.0;

    while (1) {

        std::chrono::system_clock::time_point newTime = std::chrono::high_resolution_clock::now();
        double frameTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(newTime - currentTime).count();

        if (frameTime > (1.0 / 15.0) * 1000.0) {
            frameTime = (1.0 / 15.0) * 1000.0; // note: max frame time to avoid spiral of death
        }
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= dt) {
            m_world->update(dt);
            printf("Server ms/frame is %f ms\n", dt);

            t += dt;
            accumulator -= dt;
        }

        const double alpha = accumulator / dt;

        poll();
        // do network shit
        // sleep so we don't burn cpu
        std::chrono::milliseconds timeUntilNextFrame(int(dt - accumulator));
        QThread::msleep(timeUntilNextFrame.count());
    }
}

void Server::poll()
{
    ENetEvent event;
    int eventStatus;

    while (enet_host_service(m_server, &event, 0)) {

        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "Received a new peer, adding to client list, connection from host:  " << event.peer->address.host << " at port: " << event.peer->address.port << " client has not yet been validated.";
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "client count, before adding: " << m_clients.size();

            //NOTE: we don't actually act on it, first we wait for them to send us a packet then we validate it and if so we add it to our client list
           //FIXME: probably should timeout if they're not validated within n seconds, that way they can't just keep piling on top of us, which would be a trivial malicious thing to do..

            // in debug mode, so set an insane timeout value
            if (Settings::instance()->startupFlags() & Settings::NoTimeoutStartupFlag) {
                Debug::log(Debug::ImportantArea) << "server reports an initial peer connection; setting timeout to absurdity as --no-timeout is set.";
                enet_peer_timeout(event.peer, 0, 3600 * 1000, 3600 * 1000);
            }

            //DEFAULT IS 5000
            //event.peer->timeoutMinimum = 2000;
            ////DEFAULT IS 30000
            //event.peer->timeoutMaximum = 8000;
            //event.peer->timeoutLimit = 20;
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            processMessage(event);
            break;

        case ENET_EVENT_TYPE_DISCONNECT: {
            Debug::log(Debug::Area::NetworkServerContinuousArea) << "Peer has disconnected:  " << event.peer->address.host << " at port: " << event.peer->address.port;
            printf("%s disconnected.\n", event.peer->data);

            bool found = false;
            for (auto& client : m_clients) {
                assert(client.first);
                assert(client.second);
                if (client.first == event.peer) {
                    Debug::log(Debug::Area::NetworkServerContinuousArea) << "Found peer for disconnect, deleting it";
                    m_clients.erase(client.first);
                    found = true;
                }
            }

            if (!found) {
                    Debug::log(Debug::Area::NetworkServerContinuousArea) << "Did not find peer for disconnect, something must have deleted it sooner. Should be safe to ignore."
                    << " likely scenario was that the player was kicked during pre-join";
            }

            Debug::log(Debug::Area::NetworkServerContinuousArea) << "m_clients size is now at: " << m_clients.size();

            // Reset client's information
            event.peer->data = NULL;
            break;
        }

        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

void Server::processMessage(ENetEvent& event)
{
    //std::cout << "(Server) Message from client : " << event.packet->data << "\n";
    //std::cout << "(Server) Message from client, our client->server round trip latency is: " << event.peer->roundTripTime  << "\n";
    //std::cout << "(Server) latency is: " << event.peer->lowestRoundTripTime  << "\n";

    std::string packetContents = std::string(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);

    uint32_t packetType = Packet::deserializePacketType(packetContents);

    switch (packetType) {
    case Packet::FromClientPacketContents::InitialConnectionDataFromClientPacket: {
        //checking for version mismatch, can't let him connect or else we'll have assloads of problems, among other things
        receiveInitialClientData(packetContents, event.peer);
    }

    case Packet::FromClientPacketContents::ChatMessageFromClientPacket:
        receiveChatMessage(packetContents, m_clients[event.peer]);
        break;

    case Packet::FromClientPacketContents::PlayerMoveFromClientPacket:
        receivePlayerMove(packetContents, m_clients[event.peer]);
        break;

    case Packet::FromClientPacketContents::PlayerMouseStateFromClient:
        receivePlayerMouseState(packetContents, m_clients[event.peer]);
        break;

    case Packet::FromClientPacketContents::QuickBarInventorySelectSlotRequestFromClient:
        receiveQuickBarInventorySelectSlotRequest(packetContents, m_clients[event.peer]);
        break;
    }

    enet_packet_destroy(event.packet);
}

void Server::receiveInitialClientData(const std::string& packetContents, ENetPeer* peer)
{
    PacketBuf::ClientInitialConnection message;
    Packet::deserialize(packetContents, &message);

    const int32_t major = message.versionmajor();
    const int32_t minor = message.versionminor();

    const QString playerName = QString::fromStdString(message.playername());

    Debug::log(Debug::Area::NetworkServerInitialArea) << "receiving client's player name and version data. Name: " << message.playername()
    << " version major: " << major << " minor: " << minor << " note our server version is: major: " << ore_infinium_VERSION_MAJOR << " minor: " << ore_infinium_VERSION_MINOR;

    if (major != ore_infinium_VERSION_MAJOR || minor != ore_infinium_VERSION_MINOR) {
        const QString clientVersion = QString::number(major) + "." + QString::number(minor);
        const QString serverVersion = QString::number(ore_infinium_VERSION_MAJOR) + "." + QString::number(ore_infinium_VERSION_MINOR);

        //NOTE: player has not yet been created, only a peer exists, no need to delete a player
        const QString reason = "Player pre-join kicked for version mismatch. Client version: " + clientVersion + " Server version: " + serverVersion;

        kickClient(peer, reason, Packet::ConnectionEventType::DisconnectedVersionMismatch);
        return;
    }

    //trying to trick us into using a blank name
    //TODO: perform other player name validation (e.g. only certain chars allowed), sanitize it.
    //TODO: standardize the client and server side checks? otherwise code dupes..
    if (playerName.isEmpty()) {
        //NOTE: player has not yet been created, only a peer exists, no need to delete a player
        kickClient(peer, QString("Player pre-join kicked for invalid player name"), Packet::ConnectionEventType::DisconnectedInvalidPlayerName);
        return;
    }

    // all initial validation checks have passed to connect the player to the actual server..begin doing so, and then sending initial data
    m_clients[peer] = createPlayer(playerName.toStdString());

    //he's good to go, validation succeeded, tell everyone, including himself that he joined
    for (auto& client : m_clients) {
        sendInitialPlayerData(client.first, m_clients[peer]);
    }

    for (auto& client : m_clients) {
        // now we have to send this new client every player we know about so far, except not himself (don't send his own player, obviously,
        // he already knows what it is) since we already sent that first.
        if (client.first != peer) {
            sendInitialPlayerData(peer, client.second);
        }
    }

    sendInitialPlayerDataFinished(peer);
    //HACK: FIXME:  unneeded            sendLargeWorldChunk(event.peer);

    // tell our (this) player/client what his quickbar inventory contains (send all items within it)
    uint8_t maxIndex = m_clients[peer]->quickBarInventory()->maxEquippedSlots();
    for (uint8_t index = 0; index < maxIndex; ++index) {
        sendPlayerQuickBarInventory(m_clients[peer], index);
    }

    sendWorldTime(peer);
    sendInitialVegetationSpawn(peer);
}

void Server::kickPlayer(Entities::Player* player, const QString& reason, uint32_t disconnectFlag)
{
    ENetPeer* peer = peerForPlayer(player);
    m_clients.erase(peer);
    m_world->removePlayer(player);
    delete player;

    kickClient(peer, reason, disconnectFlag);
}

void Server::kickClient(ENetPeer* peer, const QString& reason, uint32_t disconnectFlag)
{
    Debug::log(Debug::NetworkServerContinuousArea) << "kicking client, reason: " << reason.toStdString();
    enet_peer_disconnect_now(peer, disconnectFlag);
}

void Server::receiveChatMessage(const std::string& packetContents, Entities::Player* player)
{
    PacketBuf::ChatMessageFromClient receiveMessage;
    Packet::deserialize(packetContents, &receiveMessage);

    //TODO: sanitize chat message, only allow certain sets of chars, etc.

    sendChatMessage(receiveMessage.message(), player->name());
}

void Server::receivePlayerMove(const std::string& packetContents, Entities::Player* player)
{
    PacketBuf::PlayerMoveFromClient message;
    Packet::deserialize(packetContents, &message);

    int32_t x = message.directionx();
    int32_t y = message.directiony();

    if ((x >= -1 && x <= 1 && y >= -1 && y <= 1) == false) {
        const QString reason = "Client sent malicious player move packet (direction x or y is not in bounds of -1 to 1, integer). Kicking client.";
        kickPlayer(player, reason, Packet::ConnectionEventType::DisconnectedMaliciousIntent);
    }

    player->move(message.directionx(), message.directiony());
    if (message.jump()) {
        player->jump();
    }
}

void Server::receivePlayerMouseState(const std::string& packetContents, Entities::Player* player)
{
    PacketBuf::PlayerMouseStateFromClient message;
    Packet::deserialize(packetContents, &message);

    player->setMouseLeftButtonHeld(message.leftbuttonheld());
    player->setMouseRightButtonHeld(message.rightbuttonheld());
    player->setMousePositionWorldCoords(message.x(), message.y());
}

void Server::receiveQuickBarInventorySelectSlotRequest(const std::string& packetContents, Entities::Player* player)
{
    PacketBuf::QuickBarInventorySelectSlotRequestFromClient message;
    Packet::deserialize(packetContents, &message);

    const uint32_t index = message.index();

    if (index > player->quickBarInventory()->maxEquippedSlots()) {
        Debug::log(Debug::Area::ServerInventoryArea) << "server told to equip a quickbar inventory slot, but index was greater than maxEquippedSlots. this is likely a sync failure(or malicious intent).";
        return;
    }

    player->quickBarInventory()->selectSlot(index);
}

void Server::sendChatMessage(const std::string& message, const std::string& playerName)
{
    PacketBuf::ChatMessageFromServer sendMessage;
    sendMessage.set_playername(playerName);
    sendMessage.set_message(message);

    Packet::sendPacketBroadcast(m_server, &sendMessage, Packet::FromServerPacketContents::ChatMessageFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialPlayerData(ENetPeer* peer, Entities::Player* player)
{
    PacketBuf::InitialPlayerDataFromServer message;
    message.set_playername(player->name());
    message.set_playerid(player->playerID());
    message.set_x(player->position().x);
    message.set_y(player->position().y);

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::InitialPlayerDataFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialPlayerDataFinished(ENetPeer* peer)
{
    PacketBuf::InitialPlayerDataFinishedFromServer message;

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::InitialPlayerDataFinishedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendLargeWorldChunkForPlayer(Entities::Player* player)
{
    sendLargeWorldChunk(peerForPlayer(player));
}

void Server::sendLargeWorldChunk(ENetPeer* peer)
{
    PacketBuf::Chunk message;

    Debug::log(Debug::NetworkServerInitialArea) << "Server::sendLargeWorldChunk..";

    Entities::Player* player = m_clients[peer];

    assert(player->position().x >= 0.0);
    assert(player->position().y >= 0.0);

    int32_t centerX = static_cast<int32_t>(player->position().x / Block::BLOCK_SIZE);
    int32_t centerY = static_cast<int32_t>(player->position().y / Block::BLOCK_SIZE);


    Debug::log(Debug::NetworkServerInitialArea) << "centerX: " << centerX << " Y: " << centerY;
    Debug::log(Debug::NetworkServerInitialArea) << "player position x: " << player->position().x << " y: " << player->position().y;

    int32_t maxWidth = static_cast<int32_t>(MAX_VIEWPORT_WIDTH * 0.5);
    int32_t maxHeight = static_cast<int32_t>(MAX_VIEWPORT_HEIGHT * 0.5);

    //FIXME: use globals.h viewport max tiles
    int32_t startX = std::max(centerX - maxWidth, 0);
    int32_t endX = (centerX) + maxWidth;

    int32_t startY = std::max(centerY - maxHeight, 0);
    int32_t endY = (centerY) + maxHeight;

    player->lastLoadedChunk = glm::ivec2(centerX, centerY);

    Debug::log(Debug::NetworkServerInitialArea) << " INITIAL CHUNK: startx: " << startX << " endX: " << endX << " starty: " << startY << " endY: " << endY;

    message.set_startx(startX);
    message.set_endx(endX);

    message.set_starty(startY);
    message.set_endy(endY);

    Chunk chunk = m_world->createChunk(startX, startY, endX, endY);
    assert(startX < endX && startY < endY);

    for (int32_t row = startY; row < endY; ++row) {
        for (int column = startX; column < endX; ++column) {

            int32_t index = column * WORLD_ROWCOUNT + row;
            Block& block = chunk.blocks()->at(index);

            message.add_meshtype(block.meshType);
            message.add_primitivetype(block.primitiveType);
            message.add_walltype(block.wallType);
        }
    }

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::ChunkFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendWorldChunk(Chunk* chunk)
{
    PacketBuf::Chunk message;

    message.set_startx(chunk->startX());
    message.set_endx(chunk->endX());

    message.set_starty(chunk->startY());
    message.set_endy(chunk->endY());

    for (uint32_t row = chunk->startY(); row < chunk->endY(); ++row) {
        for (uint32_t column = chunk->startX(); column < chunk->endX(); ++column) {

            uint32_t index = column * WORLD_ROWCOUNT + row;
            Block& block = chunk->blocks()->at(index);

            message.add_meshtype(block.meshType);
            message.add_primitivetype(block.primitiveType);
            message.add_walltype(block.wallType);
        }
    }

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::ChunkFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

/// search for the client associated with this player
ENetPeer* Server::peerForPlayer(Entities::Player* player)
{
    ENetPeer* peer = nullptr;
    for (auto& c : m_clients) {
        if (c.second == player) {
            peer = c.first;
            break;
        }
    }

    assert(peer);
    return peer;
}

void Server::sendItemSpawned(Item* item)
{
    PacketBuf::Item message;

    message.set_x(item->position().x);
    message.set_y(item->position().y);
    message.set_itemtype(item->type());
    message.set_itemdetails(item->details());
    message.set_itemname(item->name());
    message.set_itemstate(item->state());
    message.set_stacksize(item->stackSize());
    //NOTE: index is not set, as it is not in a container of any kidn.

    switch (item->type()) {
    case Item::ItemType::Torch:
        Torch* torch = dynamic_cast<Torch*>(item);
        message.set_radius(torch->radius());
        break;
    }

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::ItemSpawnedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendQuickBarInventoryItemCountChanged(Entities::Player* player, uint8_t index, uint8_t newCount)
{
    PacketBuf::ItemCountChanged message;
    message.set_index(index);
    message.set_newcount(newCount);

    Packet::sendPacket(peerForPlayer(player), &message, Packet::FromServerPacketContents::QuickBarInventoryItemCountChangedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

Entities::Player* Server::createPlayer(const std::string& playerName)
{
    Entities::Player* player = new Entities::Player("player1Standing1");
    player->setName(playerName);
    player->setPlayerID(m_freePlayerID);

    if (m_worldViewingEnabled) {
        player->enableNoClip();
    }

    //TODO:make better server player first-spawning code
    //TODO: (much later) make it try to load the player position from previous world data, if any.
    float posX = 2600.0f / PIXELS_PER_METER;
    float posY = 14 * Block::BLOCK_SIZE; //start at the overground
    Debug::log(Debug::Area::NetworkServerInitialArea) << "CREATING PLAYER, SETTING PLAYER POS X : " << posX << " Y : " << posY;

    //HACK FIXME: HOLY FUCK BACKMAN this is fucked horribly until physics integration is 100% complete. both of these have to be at the same position, and that simpyl shouldn't be needed..
    // if you don't set oen of them, BAD SHIT HAPPENS
    player->setPosition(posX, posY);
    player->createPhysicsBody(m_world, glm::vec2(posX, posY));

    Debug::log(Debug::Area::NetworkServerInitialArea) << " POS IS: " << player->position().x << " Y: " << player->position().y;

    QuickBarInventory* quickBarInventory = new QuickBarInventory();
    player->setQuickBarInventory(quickBarInventory);

    //TODO: load the player's inventory from file..for now, initialize *the whole thing* with bullshit
    Torch *torch = new Torch(glm::vec2(0, 0));
    torch->setStackSize(64);
    quickBarInventory->setSlot(0, torch);

    Torch *torch2 = new Torch(glm::vec2(0, 0));
    torch2->setStackSize(64);
    quickBarInventory->setSlot(1, torch2);

    Tool *tool = new Tool(glm::vec2(0, 0));
    tool->setToolType(Tool::ToolType::PickAxe);
    tool->setToolMaterial(Tool::ToolMaterial::Wood);
    quickBarInventory->setSlot(2, tool);

    m_world->addPlayer(player);

    ++m_freePlayerID;

    return player;
}

void Server::sendPlayerMove(Entities::Player* player)
{
    PacketBuf::PlayerMoveFromServer message;
    message.set_playerid(player->playerID());
    message.set_x(player->position().x);
    message.set_y(player->position().y);

    Debug::log(Debug::NetworkServerContinuousArea) << "Sending player move, position x: " << player->position().x << " y: " << player->position().y;

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::PlayerMoveFromServerPacket, ENET_PACKET_FLAG_UNSEQUENCED);
}

void Server::sendWorldTimeChanged()
{
    const Time& time = m_world->worldTime();

    PacketBuf::WorldTimeChangedFromServer message;
    message.set_hour(time.hour());
    message.set_minute(time.minute());
    message.set_second(time.second());

    Packet::sendPacketBroadcast(m_server, &message, Packet::FromServerPacketContents::WorldTimeChangedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendWorldTime(ENetPeer* peer)
{
    const Time& time = m_world->worldTime();

    PacketBuf::WorldTimeChangedFromServer message;
    message.set_hour(time.hour());
    message.set_minute(time.minute());
    message.set_second(time.second());

    Packet::sendPacket(peer, &message, Packet::FromServerPacketContents::WorldTimeChangedFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendPlayerQuickBarInventory(Entities::Player* player, uint8_t index)
{
    Item* item = player->quickBarInventory()->item(index);

    if (item == nullptr) {
        Debug::log(Debug::Area::ServerInventoryArea) << "warning, BAD SHIT HAPPENED, server tried sending a player's quickbar inventory but an element was nullptr, which means we didn't send as much as we should have, so the client is empty for this element index..VERY BAD SHIT";
        return;
    }

    PacketBuf::Item message;
    //NOTE: position and such are not sent, as client doesn't need to know that for inventories.
    message.set_itemtype(item->type());
    message.set_itemdetails(item->details());
    message.set_itemname(item->name());
    message.set_itemstate(item->state());
    message.set_stacksize(item->stackSize());
    message.set_index(index);

    switch (item->type()) {
    case Item::ItemType::Torch: {
        Torch* torch = dynamic_cast<Torch*>(item);
        message.set_radius(torch->radius());
        break;
    }

    case Item::ItemType::Tool: {
        Tool* tool = dynamic_cast<Tool*>(item);
        message.set_tooltype(tool->toolType());
        message.set_material(tool->toolMaterial());
    }
    }

    // search for the client associated with this player
    ENetPeer* peer = nullptr;
    for (auto& c : m_clients) {
        if (c.second == player) {
            peer = c.first;
            break;
        }
    }

    Packet::sendPacket(peer, &message, Packet::QuickBarInventoryItemFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Server::sendInitialVegetationSpawn(ENetPeer* peer)
{
    QSet<Sprite*> results;
    m_world->m_treesSpatialHash->queryRange(&results, 0.0, 0.0, Block::BLOCK_SIZE * WORLD_COLUMNCOUNT, Block::BLOCK_SIZE * WORLD_ROWCOUNT);

    PacketBuf::InitialVegetationSpawn message;

    for (Sprite* sprite : results) {
        const glm::vec2& pos = sprite->position();
        message.add_x(pos.x);
        message.add_y(pos.y);
    }
    Packet::sendPacket(peer, &message, Packet::InitialVegetationSpawningFromServerPacket, ENET_PACKET_FLAG_RELIABLE);
}
