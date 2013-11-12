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

#ifndef SERVER_H
#define SERVER_H

#include <enet/enet.h>

#include <src/player.h>

#include <string>
#include <map>

#include <QString>

class Item;
class World;
class Player;
class Chunk;
class Client;
class Time;

class Server
{
public:
    Server();
    ~Server();

    void init(uint8_t maxClients, uint16_t port = DEFAULT_PORT, Client* client = nullptr);

    void enableWorldViewing() {
        m_worldViewingEnabled = true;
    }

    void poll();
    void tick();
    void processMessage(ENetEvent& event);

    static constexpr int MAXPLAYERS = 8;

    /**
     * Returns the client that started this server. This is ONLY VALID if it's a self-hosted server/client session.
     * Use world's m_client for indication that the world is in client mode, instead.
     */
    Client* client() {
        return m_client;
    }

    /**
     * Broadcast to clients that this player's position has changed
     */
    void sendPlayerMove(Entities::Player* player);

    /**
     * Broadcast a chunk change.
     */
    void sendWorldChunk(Chunk* chunk);
    void sendItemSpawned(Item* item);
    void sendQuickBarInventoryItemCountChanged(Entities::Player* player, uint8_t index, uint8_t newCount);

    /**
     * Broadcast when the time changes outside of its regular tick interval. Basically, the clients, when they first connect, get the time
     * and assume it keeps going at a steady rate. If there's some kind of in-game item, or the admin wants to change the time,
     * then those are the scenarios that this will be needed.
     */
    void sendWorldTimeChanged();

    /**
     * Should be called when player's quickbarinventory has been modified at @p index,
     * as it will then send this to the client associated with this player, with the new up-to-date item.
     */
    void sendPlayerQuickBarInventory(Entities::Player* player, uint8_t index);

    /**
     * Sends a big (greater than viewport-sized) chunk to the player and updates the classes
     * internal storage of where the last loaded chunk was.
     *
     * This should be called when the player reaches a certain distance from the last loaded chunk for that client,
     * but not too late otherwise there will be pop-in.
     *
     * NOTE: internally looks up the peer for player and calls @sa sendLargeWorldChunk
     */
    void sendLargeWorldChunkForPlayer(Entities::Player* player);

    void kickPlayer(Entities::Player* player, const QString& reason, uint32_t disconnectFlag);

private:
    void kickClient(ENetPeer* peer, const QString& reason, uint32_t disconnectFlag);

    /**
     * Processes all data that the client sends when it first connects. Checks the client for validation,
     * which includes player name checking, version checking, etc.
     *
     * If it passes validation, it begins sending initial data from the server (world data and the like).
     *
     */
    void receiveInitialClientData(const std::string& packetContents, ENetPeer* peer);
    void receiveChatMessage(const std::string& packetContents, Entities::Player* player);
    void receivePlayerMove(const std::string& packetContents, Entities::Player* player);
    void receivePlayerMouseState(const std::string& packetContents, Entities::Player* player);
    void receiveQuickBarInventorySelectSlotRequest(const std::string& packetContents, Entities::Player* player);

    void sendChatMessage(const std::string& message, const std::string& playerName);
    void sendInitialPlayerData(ENetPeer* peer, Entities::Player* player);
    void sendInitialPlayerDataFinished(ENetPeer* peer);
    void sendInitialVegetationSpawn(ENetPeer* peer);
    void sendLargeWorldChunk(ENetPeer* peer);
    void sendWorldTime(ENetPeer* peer);


    /**
     * Small helper function to iterate through the list of peers and players, if all you have is a player, then find the peer.
     */
    ENetPeer* peerForPlayer(Entities::Player* player);

    Entities::Player* createPlayer(const std::string& playerName);

private:
    /**
     * Will always be the ID after the last used one, so it's always unique and
     * unused and will gradually rise as the player connection count history goes up.
     */
    uint32_t m_freePlayerID = 0;

    World* m_world = nullptr;

    Client* m_client = nullptr;

    /**
     * For debug scenarios, see settings debug flag
     */
    bool m_worldViewingEnabled = false;

private:
    ENetHost* m_server = nullptr;
    ENetAddress m_address;
    std::map<ENetPeer*, Entities::Player*> m_clients;
};

#endif
