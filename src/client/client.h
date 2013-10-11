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

#include <QtCore/QQueue>
#include <QtCore/QTime>
#include <QtCore/QMutex>

#include <SDL2/SDL.h>
#include <SDL_log.h>

#include <thread>

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
class QKeyEvent;

class Client : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

    qreal t() const { return m_t; }
    void setT(qreal t);

public:

     Q_INVOKABLE int defaultPort() {
        return DEFAULT_PORT;
    }


signals:
    void tChanged();

public slots:
    void paintUnder();
    void cleanup();
    void sync();

    void init();

    void viewKeyPressed(QKeyEvent* event);

    //------------------ from QML ...
    Q_INVOKABLE void startSingleplayerCreateSlot(const QString& playerName, const QString& worldName);
    Q_INVOKABLE void startMultiplayerHostSlot(const QString& playerName, int port);
    Q_INVOKABLE void startMultiplayerJoinSlot(const QString& playerName, const QString& address, int port);
    Q_INVOKABLE void exitClicked();


    //-------------------

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void processSharedKeyEvents();

    QTime m_time;
    int m_frameCount = 0;

    qreal m_t = 0.0;
    qreal m_thread_t = 0.0;
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

    void handlePlayerInput(SDL_Event& event);

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

    void handleInputEvents();
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

    SDL_Window *m_window = nullptr;
    SDL_GLContext m_GLcontext;

    std::string m_playerName;

    Entities::Player* m_mainPlayer = nullptr;

    std::thread* m_serverThread = nullptr;
    std::thread* m_clientTickLogicThread = nullptr;

    /**
     * For the client, there are a few threads, most are from Qt.
     * There's the Qt main thread, the qtquick rendering thread (which runs all of our GL commands),
     * and the client logic thread. The client logic thread handles networking and the interaction with game objects therein.
     *
     * The client logic thread never touches rendering code (obviously, as the GL is only on the qtquick rendering thread).
     *
     * Upon key presses, qt will handle it in a non-rendering thread (which I believe I've found to be a special thread, with no event loop).
     *
     * We will then lock this mutex and modify some data and of course, unlock it upon return.
     * On tick of the client logic thread, @sa tickLogicThread
     * this lock will be grabbed as we will be modifying game state when we poll the network and game objects and push and pull them.
     *
     * The renderer thread will also be grabbing said lock prior to rendering.
     *
     * TODO: what i *really* want to do is form a task scheduler that can process jobs and place them in a job queue. Few games do this though,
     * mostly just idTech4,5. Even Unreal doesn't do that, last I checked, it has dedicated threads for subsystems. But that means you'll have e.g.
     * 2 really slow subsystems stalling everything else. This happened with Supreme Commander which is why it runs like shit on n-core systems,
     * but less shit on dual core systems.
     *
     */
    QMutex m_clientLogicMutex;

    bool m_connected = false;

    int32_t m_playerInputDirectionX = 0;
    int32_t m_playerInputDirectionY = 0;

    cpSpace* m_cpSpace = nullptr;
    PhysicsDebugRenderer* m_physicsDebugRenderer = nullptr;

    bool m_playerJumpRequested = false;
    bool m_renderGUI = true;
    bool m_initialPlayersReceivedFinished = false;

    bool m_worldViewingEnabled = false;

    bool m_firstGLInit = false;

    QuickView* m_view = nullptr;

private:
    ENetHost* m_client = nullptr;

    /// server
    ENetPeer* m_peer = nullptr;
    /// server address
    ENetAddress m_address;
};

#endif
