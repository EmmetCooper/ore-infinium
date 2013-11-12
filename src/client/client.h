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

#ifndef CLIENT_H
#define CLIENT_H

#include "glew.h"

#include <enet/enet.h>
#include <string>

#include "src/player.h"

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QTime>
#include <QMutex>

#include <atomic>
#include <mutex>
#include <thread>

class QQuickView;
class FboInSGRenderer;
struct cpSpace;
class DebugSettings;
class PhysicsDebugRenderer;
class QuickBarMenu;
class Player;
class GUI;
class MainMenu;
class ChatDialog;
class DebugMenu;
class World;
class Server;
class QuickView;
class QQuickWindow;

class Client : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(double frameTime READ frameTime NOTIFY frameTimeChanged);
    Q_PROPERTY(bool gameConnected READ gameConnected NOTIFY gameConnectedChanged);

public:

    bool gameConnected() {
        return m_connected;
    }

    double frameTime() {
        return m_frameTime;
    }

     Q_INVOKABLE int defaultPort() {
        return DEFAULT_PORT;
    }

    void paint();

signals:
    /**
     * Hides the main menu, also used for debug. QML hooks into this.
     */
    void gameStarted();
    void gameConnectedChanged();

    void frameTimeChanged(double frameTime);

public slots:
    void cleanup();
    void sync();

    void init();

    //------------------ from QML ...
    Q_INVOKABLE void startSingleplayerCreateSlot(const QString& playerName, const QString& worldName);
    Q_INVOKABLE void startMultiplayerHostSlot(const QString& playerName, int port);
    Q_INVOKABLE void startMultiplayerJoinSlot(const QString& playerName, const QString& address, int port);
    Q_INVOKABLE void disconnectClicked();
    Q_INVOKABLE void exitClicked();
    //-------------------

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    QTime m_time;
    int m_frameCount = 0;

    //FIXME: ////////////////////////////////////////////////// UGLY, REFACTOR

public:
    Client();
    ~Client();

    void startSinglePlayer(const std::string& playername);
    /**
     * Starts a mutliplayer client only connection, aka connecting to a server, not hosting.
     * @p address the server address/IP to connect to
     */
    bool startMultiplayerClientConnection(const std::string& playername, const char* address, uint16_t port = DEFAULT_PORT);
    /**
     * Similar to @sa startSinglePlayer
     * but instead hosts a multiplayer game, presumably visible to others through internet/LAN ip entering.
     */
    void startMultiplayerHost(const std::string& playername, uint16_t port = DEFAULT_PORT);

    void tick(double frameTime);
    void render(double frameTime);

    void viewKeyPressed(QKeyEvent* event);

    void enableWorldViewing() {
       m_worldViewingEnabled = true;
    }

    const float FPS = 60.0;

    PhysicsDebugRenderer* physicsDebugRenderer() {
        return m_physicsDebugRenderer;
    }

    void setActiveChunkCount(uint32_t count);

    void disconnect();

    void shutdown();

    bool connected() {
        return m_connected;
    }

    bool hosting() {
        return m_server;
    }

    Entities::Player* mainPlayer() {
        return m_mainPlayer;
    }

    World* world() {
        return m_world;
    }

///////////////// Network Communication ////////////////////
public:
    void sendChatMessage(const std::string& message);

    /**
     * Called each tick, informs server of the player mouse position and button state,
     * *AND* sets the m_mainPlayer's mouse state to the present state. The latter is important
     * for the world to be able to generalize things regardless if in client or server mode. (e.g. picking vs. crosshair rendering)
     * Sends mouse position in world coordinates, since client coordinates are useless to the server. (since it has no cameras and so
     * doesn't know how to unproject them)
     */
    void sendPlayerMouseState();

    void sendQuickBarInventorySlotSelectRequest(uint8_t index);

private:

    void tickLogicThread();

    /**
     * blocks until it can connect within a timeout.
     * @returns true on success, false on failure
     */
    bool connectTo(const char* address = "127.0.0.1", uint16_t port = DEFAULT_PORT);
    void sendInitialConnectionData();
    void sendPlayerMovement();

    void poll();

    void processMessage(ENetEvent& event);

    void receiveChatMessage(const std::string& packetContents);
    void receiveInitialPlayerData(const std::string& packetContents);
    void receivePlayerDisconnected(const std::string& packetContents);
    void receivePlayerMove(const std::string& packetContents);
    void receiveChunk(const std::string& packetContents);
    void receiveItemSpawned(const std::string& packetContents);
    void receiveWorldTimeChanged(const std::string& packetContents);
    void receiveQuickBarInventoryItem(const std::string& packetContents);
    void receiveQuickBarInventoryItemCountChanged(const std::string& packetContents);
    void receiveInitialVegetationSpawning(const std::string& packetContents);

///////////////////////////////////////////////

private:
    void initGL();

    glm::vec2 mousePositionToWorldCoords();

    void drawDebugText(double frameTime);

private:
    Server* m_server = nullptr;
    World* m_world = nullptr;

    GUI* m_gui = nullptr;
    MainMenu* m_mainMenu = nullptr;
    ChatDialog* m_chat = nullptr;
    DebugMenu* m_debugMenu = nullptr;
    QuickBarMenu* m_quickBarMenu = nullptr;
    DebugSettings* m_debugSettings = nullptr;

    std::string m_playerName;

    Entities::Player* m_mainPlayer = nullptr;

    std::thread* m_serverThread = nullptr;
    std::thread* m_clientTickLogicThread = nullptr;
    bool m_connected = false;

    /////////////////////////
    QMutex m_playerInputLock;
    int m_playerInputDirectionX = 0;
    int m_playerInputDirectionY = 0;

    bool m_playerJumpRequested = false;
    /////////////////////////

    cpSpace* m_cpSpace = nullptr;
    PhysicsDebugRenderer* m_physicsDebugRenderer = nullptr;

    bool m_renderGUI = true;
    bool m_initialPlayersReceivedFinished = false;

    bool m_worldViewingEnabled = false;

    bool m_firstGLInit = false;

    double m_frameTime = 0.0;

private:
    ENetHost* m_client = nullptr;

    /// server
    ENetPeer* m_peer = nullptr;
    /// server address
    ENetAddress m_address;
};

#endif
