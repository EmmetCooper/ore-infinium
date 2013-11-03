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

#include "client.h"

#include "src/client/FboInSGRenderer.h"

#include "src/network/packet.h"
#include "src/packet.pb.h"
#include "src/server/server.h"


#include "src/physicsdebugrenderer.h"

#include "gui/debugsettings.h"
#include "gui/optionsdialogbackend.h"

#include "src/settings/settings.h"

#include "src/quickview.h"
#include "src/quickbarinventory.h"
#include "src/world.h"
#include "src/player.h"
#include "src/camera.h"
#include "src/debug.h"
#include <src/item.h>
#include <src/torch.h>
#include <src/tool.h>
#include <src/vegetation.h>
#include "src/../config.h"

#include <random>

#include <QQuickView>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <QOpenGLContext>
#include <QThread>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_events.h>

Client::Client(QQuickView* view)
: m_view(view)
{
}

Client::~Client()
{
    enet_host_destroy(m_client);

    delete m_mainPlayer;
}

void Client::init()
{
    Debug::log(Debug::ImportantArea) << "CLIENT INIT START!";

    // FIXME:
    // glViewport(0, 0, Settings::instance()->windowWidth, Settings::instance()->windowHeight);

    // m_debugMenu = new DebugMenu(this);
    // m_debugMenu->show();

    Debug::log(Debug::ImportantArea) << "CLIENT INIT END!";
}

void Client::tickLogicThread()
{
    /*
    Debug::log(Debug::ImportantArea) << "CLIENT EXEC!";
    Debug::log(Debug::ImportantArea) << "GAME TICK!";

    std::chrono::system_clock::time_point currentTime = std::chrono::high_resolution_clock::now();

    double accumulator = 0.0;
    const double dt = (1.0 / 60.0) * 1000.0; // runs at 60 hz
    double t = 0.0;

    double fps = 0.0;
    //FIXME: obviously...
    while (1) {

    std::chrono::system_clock::time_point newTime = std::chrono::high_resolution_clock::now();
    double frameTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli> >(newTime - currentTime).count();

    if (frameTime > (1.0 / 15.0) * 1000.0) {
        frameTime = (1.0 / 15.0) * 1000.0; // note: max frame time to avoid spiral of death
    }
    currentTime = newTime;

    accumulator += frameTime;

    while (accumulator >= dt) {
        t += dt;
        accumulator -= dt;

        Debug::log(Debug::ImportantArea) << "TICKING LOGIC THREAD AND SUB TICK!";
        tick(frameTime);
    }

//    render(frameTime);

    const double alpha = accumulator / dt;

    // sleep so we don't burn cpu
    //  std::chrono::milliseconds timeUntilNextFrame(int(dt - accumulator));
    //  std::this_thread::sleep_for(timeUntilNextFrame);
    }
    */
}

void Client::initGL()
{
//    Debug::log(Debug::Area::StartupArea) << "Compiled against SDL version: " << int(compiled.major) << "." << int(compiled.minor) << "-" << int(compiled.patch) <<
//                                         " Running (linked) against version: " << int(linked.major) << "." << int(linked.minor) << "-" << int(linked.patch);
//
//    m_window = SDL_CreateWindow("Ore Infinium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
//                                Settings::instance()->windowWidth, Settings::instance()->windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
//    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//
//    // Request opengl 3.3 context.
//    // FIXME: i *want 3.2, but Mesa 9 only has 3.0.. :(
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
//
//    /* Turn on double buffering with a 24bit Z buffer.
//     * You may need to change this to 16 or 32 for your system */
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   glewExperimental = true;

    int retGLEW = glewInit();

    Debug::log(Debug::Area::StartupArea) << "glewInit return code: " << retGLEW;

    for(int i = 0; i < 100; ++i) {
      Debug::checkGLErrorSafe();
    }

    Debug::checkGLError();


    Debug::assertf(retGLEW == GLEW_OK, "glewInit returned !GLEW_OK. No GL context can be formed..bailing out");

    Debug::checkGLError();


    //FIXME: MOVE INTO INITGL
    Debug::checkGLError();

    Debug::log(Debug::Area::StartupArea) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
    Debug::log(Debug::Area::StartupArea) << "Platform: Renderer: " << glGetString(GL_RENDERER);
    Debug::log(Debug::Area::StartupArea) << "OpenGL Version: " << glGetString(GL_VERSION);
    Debug::log(Debug::Area::StartupArea) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
    Debug::log(Debug::Area::StartupArea) << "Built against Qt version: " << QT_VERSION_STR;
    Debug::log(Debug::Area::StartupArea) << "Running against Qt version: " << qVersion();

    Debug::checkGLError();

    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::StartupArea) << "Maximum OpenGL texture size allowed: " << textureSize << "\n\n\n";

#ifdef GLEW_KHR_debug
    if (GLEW_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(&Debug::glDebugCallback, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    } else {
        Debug::log(Debug::Area::ImportantArea) << "GLEW_KHR_debug is not available, disabling OpenGL debug reporting facilities. The extension was compiled in but is not available at runtime.";
    }
#endif

    Debug::fatal(enet_initialize() == 0, Debug::Area::ImportantArea, "An error occurred during ENet init (network init failure");

    Debug::log(Debug::Area::StartupArea) << "initGL finished";
}

void Client::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        Debug::log(Debug::ImportantArea) << "windowChanged slot hit";
        // Connect the beforeRendering signal to our paint function.
        // Since this call is executed on the rendering thread it must be
        // a Qt::DirectConnection
//        connect(win, SIGNAL(beforeRendering()), this, SLOT(paintUnder()), Qt::DirectConnection);
 //       connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
//        win->setClearBeforeRendering(false);
    }
}

void Client::paint()
{
    if (!m_firstGLInit) {
        Debug::log(Debug::ImportantArea) << "first paintunder, not init'd perform initGL";
        m_firstGLInit = true;
        initGL();

        //FIXME: not needed move into initGL if not there already.
        glViewport(0, 0, m_view->width(), m_view->height());
        Debug::log(Debug::ImportantArea) << "WIN HEIGHT, W: " << m_view->height() << " : " << m_view->width();

        if (Settings::instance()->startupFlags() & Settings::PlayNowStartupFlag) {
            emit playNowStarted();
            startMultiplayerHost("Dingo");
        }

    } else {
        glClearColor(0.5, 0.0, 0.0, 1.0);

        assert(m_view->openglContext());

        double frameTime = 0.0;
        if (m_frameCount == 0) {
            m_time.start();
        } else {
            frameTime = (m_time.elapsed() / static_cast<double>(m_frameCount));
            m_frameTime = frameTime;

            emit frameTimeChanged(frameTime);
            //printf("ms/frame is %f ms\n", frameTime);
        }

        tick(frameTime);

        render(frameTime);

        m_frameCount++;
    }

    //    glEnable(GL_BLEND);
    //   glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    //
    //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //
    //    m_program->disableAttributeArray(0);
    //    m_program->release();
}

void Client::sync()
{
}

void Client::cleanup()
{

}

void Client::exitClicked()
{
    Debug::log(Debug::ImportantArea) << "Shutting down...";
    //TODO: do something sane, and  flush configs and shit?
    shutdown();
}

void Client::startSingleplayerCreateSlot(const QString& playerName, const QString& worldName)
{
    Debug::log(Debug::ImportantArea) << "SP start slot, playername, world: " << qPrintable(playerName) << " : " << qPrintable(worldName);
}

void Client::startMultiplayerHostSlot(const QString& playerName, int port)
{
    Debug::log(Debug::ImportantArea) << "MP host slot, playername, addr, port: " << qPrintable(playerName) << " : " << port;
}

void Client::startMultiplayerJoinSlot(const QString& playerName, const QString& address, int port)
{
    Debug::log(Debug::ImportantArea) << "MP join slot, playername, addr, port: " << qPrintable(playerName) << " : " << qPrintable(address) << " : " << port;
}

///////////////////////////////////////////////////////////////////////////////////////

void Client::poll()
{
    ENetEvent event;
    int eventStatus;

    while (enet_host_service(m_client, &event, 0)) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            assert(0);
        }
        break;

        case ENET_EVENT_TYPE_RECEIVE:
            //Debug::log(Debug::Area::NetworkClient) << "Message from server, our client->server round trip latency is: " << event.peer->lastRoundTripTime;
            processMessage(event);;
            break;

        case ENET_EVENT_TYPE_DISCONNECT: {
            Debug::log(Debug::Area::NetworkClientContinuousArea) << "Peer disconnected: " << event.data;
            m_connected = false;

            switch (event.data) {
            case Packet::ConnectionEventType::DisconnectedVersionMismatch:
                Debug::log(Debug::Area::NetworkClientContinuousArea) << "Server booted us, client version does not match server version.";
                //FIXME: gracefully handle a version mismatch, obviously
                assert(0);
                break;
            case Packet::ConnectionEventType::DisconnectedInvalidPlayerName:
                Debug::log(Debug::Area::NetworkClientContinuousArea) << "Server booted us, invalid player name";
                assert(0);
                break;
            }

            char hostname[32];
            enet_address_get_host_ip(&event.peer->address, hostname, static_cast<size_t>(32));
            Debug::log(Debug::Area::NetworkClientContinuousArea) << "disconnected from server host IP: " << hostname;
            enet_peer_reset(m_peer);

            // Reset client's information
            event.peer->data = nullptr;
            delete m_peer;
            m_peer = nullptr;
        }
        break;

        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

void Client::render(double frameTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Debug::log(Debug::ImportantArea) << "CLIENT RENDER CALLED";

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    if (m_world && m_mainPlayer) {
        m_world->render();
    } else {
        //Debug::log(Debug::ImportantArea) << "NOT RENDERING, player/world invalid";
    }

//
//    // only a client-hosted server has a chance of seeing any debug shit
//    if (m_server) {
//        if (!m_physicsDebugRenderer && m_world && m_world->spriteSheetRenderer()) {
////FIXME:HACK:            m_physicsDebugRenderer = new PhysicsDebugRenderer(m_world->spriteSheetRenderer()->camera());
//            // physics debug renderer first init...
////            m_box2DWorld->SetDebugDraw(m_physicsDebugRenderer);
//        }
//
//        if (m_physicsDebugRenderer) {
//
//            int rendererFlags = 0;
//            int settingsFlags = Settings::instance()->debugRendererFlags;
//            bool drawingRequired = false;
//
//            if (settingsFlags & Debug::RenderingDebug::ChipmunkAABBRenderingDebug) {
//                // rendererFlags |= b2Draw::e_aabbBit;
//                drawingRequired = true;
//            }
//
//            if (settingsFlags & Debug::RenderingDebug::ChipmunkShapeRenderingDebug) {
//                //  rendererFlags |= b2Draw::e_shapeBit;
//                drawingRequired = true;
//            }
//
//            if (settingsFlags & Debug::RenderingDebug::ChipmunkCenterOfMassRenderingDebug) {
//                //   rendererFlags |= b2Draw::e_centerOfMassBit;
//                drawingRequired = true;
//            }
//
//            if (settingsFlags & Debug::RenderingDebug::ChipmunkJointRenderingDebug) {
//                //    rendererFlags |= b2Draw::e_jointBit;
//                drawingRequired = true;
//            }
//
//            if (drawingRequired) {
//                m_physicsDebugRenderer->render();
//            }
//        }
//    }
//
//    if (m_renderGUI) {
//  //      drawDebugText(frameTime);
// //       m_gui->render();
//    } else {
//        static int limiter = 0;
//        ++limiter;
//
//        if (limiter > 10) {
//            limiter = 0;
//            Debug::log(Debug::ImportantArea) << "FPS: " << (1000.0 / frameTime) << " FrameTime: " << frameTime << " ms";
//        }
//    }
//
}

void Client::tick(double frameTime)
{
//    handleInputEvents();

    if (m_peer) {
        poll();
    }

   if (m_world) {
       if (m_mainPlayer) {
           //NOTE: we send this shit regardless of input events..for obvious reasons. (aka fossils of a once living bug lie here ;)
           sendPlayerMovement();
           sendPlayerMouseState();
       }

       m_world->update(frameTime);

       if (m_quickBarMenu) {
//           m_quickBarMenu->update();
       }

       if (m_server) {
           if (m_physicsDebugRenderer) {
//               m_debugMenu->setPhysicsWorldShapeCount(m_physicsDebugRenderer->shapeCount());
           }
       }
   }
}

void Client::setActiveChunkCount(uint32_t count)
{
//    m_debugMenu->setActiveChunkCount(count);
}

void Client::drawDebugText(double frameTime)
{
 //   m_debugMenu->update(frameTime);
}


void Client::viewKeyPressed(QKeyEvent* event)
{
//        if (m_mainPlayer && m_peer && m_connected && m_gui->inputDemanded() == false) {
//            handlePlayerInput(event);
//            m_quickBarMenu->handleEvent(event);
//        }
return;//HACK:
    assert(event);

/////    QMutexLocker lock(&m_playerInputLock);
    std::lock_guard<std::mutex> lock(m_lock);
    m_test = 2;
    switch (event->key()) {
        case Qt::Key_Left:
            //fall through
        case Qt::Key_A:
            m_playerInputDirectionX = -1;
            break;

        case Qt::Key_Right:
            //fall through
        case Qt::Key_D:
            m_playerInputDirectionX = 1;
            break;

        case Qt::Key_Up:
            //fall through
        case Qt::Key_W:
            m_playerInputDirectionY = -1;
            break;

        case Qt::Key_Down:
            //fall through
        case Qt::Key_S:
            m_playerInputDirectionY = 1;
            break;

        case Qt::Key_Space:
            m_playerJumpRequested = true;
            break;

        case Qt::Key_L:
            //fall through
        case Qt::Key_Equal:
            if (m_world) {
                //m_world->zoomIn();
            }
            break;

        case Qt::Key_K:
            //fall through
        case Qt::Key_Minus:
            if (m_world) {
                //m_world->zoomOut();
            }
            break;

        case Qt::Key_Escape: {
            //only if we are connected, do we allow hiding and showing (escape menu)
            if (m_peer) {
                //FIXME: HACK
                event->accept();
            //     if (!m_mainMenu->escapeMenuVisible()) {
            //         m_mainMenu->showEscapeMenu();
            //     } else {
            //         m_mainMenu->hideEscapeMenu();
            //     }
            }
        }

        case Qt::Key_F1: {
            //f1
            // m_debugMenu->setCollapsed(!m_debugMenu->collapsed());
        }

        case Qt::Key_F8: {
            /* FIXME: move to main thread
            Debug::log(Debug::ImportantArea) << "KEYPRESS EVENT! accepting event, f8";
            event->accept();
            //f8
            std::stringstream ss;
            ss << "Player";
            std::random_device device;
            std::mt19937 rand(device());
            std::uniform_int_distribution<> distribution(0, INT_MAX);

            ss << distribution(rand);
            Debug::log(Debug::ImportantArea) << "keypressevent THREAD ID: " << QThread::currentThreadId();

            Debug::log(Debug::ImportantArea) << "KEYPRESS EVENT! starting mp host";
            startMultiplayerHost(ss.str());
            Debug::log(Debug::ImportantArea) << "KEYPRESS EVENT! returned to keypressevent";
            */
        }

        case Qt::Key_F10: {
            //FIXME:               m_renderGUI = !m_renderGUI;
        }

        case Qt::Key_F11: {
            //f11
            //if (m_debugSettings == nullptr) {
            //    m_debugSettings = new DebugSettings(this);
            //    m_debugSettings->show();
            //} else {
            //    if (m_debugSettings->visible()) {
            //        m_debugSettings->hide();
            //    } else {
            //        m_debugSettings->show();
            //    }
            //}
        }

        default:
            break;
    }

    m_playerInputDirectionX = -1;
    Debug::log(Debug::ImportantArea) << "KEY PRESSED VALUE X: " << m_playerInputDirectionX;
}

void Client::shutdown()
{
    if (m_connected) {
        disconnect();
    }
    //FIXME: graceful client shutdown needed..
    exit(0);
}

bool Client::connectTo(const char* address, uint16_t port)
{
    m_client = enet_host_create(nullptr /* create a client host */,
                                1 /* only allow 1 outgoing connection */,
                                2 /* allow up 2 channels to be used, 0 and 1 */,
                                57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                                14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    Debug::assertf(m_client, "failed to create ENet client host");

    enet_address_set_host(&m_address, address);
    m_address.port = port;

    m_peer = enet_host_connect(m_client, &m_address, 2, 0);

    if (Settings::instance()->startupFlags() & Settings::NoTimeoutStartupFlag) {
        enet_peer_timeout(m_peer, 0, 3600 * 1000, 3600 * 1000);
    }

    if (m_peer == NULL) {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client failed to connect to server";
        exit(EXIT_FAILURE);
    }

    ENetEvent event;
    if (enet_host_service(m_client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client connection to server succeeded!";
//        m_mainMenu->hideMainMenu();
//
//        m_chat = new ChatDialog(this, m_mainMenu);
//        m_chat->show();

        //NOTE: no world is created yet. we now wait for the server to receive our initial connection data, and give us back a
        //player id, which we then create as the main player and finally, create the world.
        m_connected = true;
        sendInitialConnectionData();
        return true;
    } else {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client connection to server failed!";
        Debug::log(Debug::Area::NetworkClientInitialArea) << "Client failed to connect to server within timeout";
        enet_peer_reset(m_peer);
        return false;
    }
}

void Client::disconnect()
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "attempting disconnect from server";
    enet_peer_disconnect(m_peer, 0);
}

void Client::startSinglePlayer(const std::string& playername)
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "starting singleplayer! Entities::Playername: " << playername;
    m_playerName = playername;

    //create a local server, and connect to it.
    m_server = new Server();
    m_server->init(1);
    m_serverThread = new std::thread(&Server::tick, m_server);
    connectTo();
}

bool Client::startMultiplayerClientConnection(const std::string& playername, const char* address, uint16_t port)
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "starting multiplayer joining address: " << address << "! Entities::Playername: " << playername;
    m_playerName = playername;

    if (connectTo(address, port)) {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "connection success!";
        return true;
    } else {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "connection failure!";
        return false;
    }
}

void Client::startMultiplayerHost(const std::string& playername, uint16_t port)
{
    Debug::log(Debug::Area::NetworkClientInitialArea) << "starting multiplayer, hosting! Entities::Playername: " << playername << " port: " << port;
    if (!m_server) {
        m_playerName = playername;

        m_server = new Server();

        if (m_worldViewingEnabled) {
            m_server->enableWorldViewing();
        }

        m_server->init(8 /* 8 players (max) */, port, this);
        m_serverThread = new std::thread(&Server::tick, m_server);
        connectTo();
    } else {
        Debug::log(Debug::Area::NetworkClientInitialArea) << "error, attempted to create player-hosted a server but we're still connected to this one";
    }
}

void Client::sendInitialConnectionData()
{
    PacketBuf::ClientInitialConnection message;
    message.set_playername(m_playerName);
    message.set_versionmajor(ore_infinium_VERSION_MAJOR);
    message.set_versionminor(ore_infinium_VERSION_MINOR);

    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::InitialConnectionDataFromClientPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Client::sendChatMessage(const std::string& message)
{
    PacketBuf::ChatMessageFromClient messagestruct;
    messagestruct.set_message(message);

    Packet::sendPacket(m_peer, &messagestruct, Packet::FromClientPacketContents::ChatMessageFromClientPacket, ENET_PACKET_FLAG_RELIABLE);
}

void Client::sendPlayerMovement()
{
    Debug::log(Debug::ImportantArea) << "SENDING PLAYER INPUT, input x: " << m_playerInputDirectionX << " Y : " << m_playerInputDirectionY;
//    QMutexLocker lock(&m_playerInputLock);
    std::lock_guard<std::mutex> lock(m_lock);

    Debug::log(Debug::ImportantArea) << "SENDING PLAYER INPUT, input x: " << m_playerInputDirectionX << " Y : " << m_playerInputDirectionY;
    if (m_test == 2) {
        exit(1);
    }

    if (m_playerInputDirectionX != 0) {
        assert(0);
    }

    Debug::log(Debug::ImportantArea) << "SENDING PLAYER INPUT, input x: " << m_playerInputDirectionX << " Y : " << m_playerInputDirectionY;
    PacketBuf::PlayerMoveFromClient message;
    message.set_directionx(m_playerInputDirectionX);
    message.set_directiony(m_playerInputDirectionY);
    message.set_jump(m_playerJumpRequested);

    Debug::log(Debug::ImportantArea) << "SENDING PLAYER INPUT, input x: " << m_playerInputDirectionX << " Y : " << m_playerInputDirectionY;
    //reset values back to initial state. that way no need to buggily handle keypress up events..
//    m_playerInputDirectionX = 0;
//    m_playerInputDirectionY = 0;
//    m_playerJumpRequested = false;

    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::PlayerMoveFromClientPacket, ENET_PACKET_FLAG_RELIABLE);
}

glm::vec2 Client::mousePositionToWorldCoords()
{
    assert(m_world);

    int x; int y;
    SDL_GetMouseState(&x, &y);

    glm::vec2 mouse = glm::vec2(x, Settings::instance()->windowHeight - y);

    glm::vec4 viewport = glm::vec4(0, 0, Settings::instance()->windowWidth, Settings::instance()->windowHeight);
    glm::vec3 wincoord = glm::vec3(mouse.x, mouse.y, 0);
    glm::vec3 unproject = glm::unProject(wincoord, m_world->camera()->view(), m_world->camera()->ortho(), viewport);

    mouse = glm::vec2(unproject.x, unproject.y);

    return mouse;
}

void Client::sendPlayerMouseState()
{
    glm::vec2 mousePosition = mousePositionToWorldCoords();

    //set the main player's mouse position, which we can then use for generalized lookups regardless if we're in server or client mode
    // (e.g. rendering crosshair vs. networked picking/block selection..both would otherwise require two different solutions but now do not)
    m_mainPlayer->setMousePositionWorldCoords(mousePosition.x, mousePosition.y);

    bool leftHeld = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(1);
    bool rightHeld = SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(3);

    PacketBuf::PlayerMouseStateFromClient message;
    message.set_x(mousePosition.x);
    message.set_y(mousePosition.y);
    message.set_leftbuttonheld(leftHeld);
    message.set_rightbuttonheld(rightHeld);


    //FIXME: make UNRELIABLE, and verify it is safe to do so..
    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::PlayerMouseStateFromClient, ENET_PACKET_FLAG_RELIABLE);
}

void Client::sendQuickBarInventorySlotSelectRequest(uint8_t index)
{
    PacketBuf::QuickBarInventorySelectSlotRequestFromClient message;
    message.set_index(index);

    Packet::sendPacket(m_peer, &message, Packet::FromClientPacketContents::QuickBarInventorySelectSlotRequestFromClient, ENET_PACKET_FLAG_RELIABLE);
}

void Client::processMessage(ENetEvent& event)
{
    std::string packetContents = std::string(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);

    uint32_t packetType = Packet::deserializePacketType(packetContents);

    switch (packetType) {
    case Packet::FromServerPacketContents::ChatMessageFromServerPacket:
        receiveChatMessage(packetContents);
        break;

    case Packet::FromServerPacketContents::InitialPlayerDataFromServerPacket:
        receiveInitialPlayerData(packetContents);
        break;

    case Packet::FromServerPacketContents::PlayerDisconnectedFromServerPacket:
        receivePlayerDisconnected(packetContents);
        break;

    case Packet::FromServerPacketContents::PlayerMoveFromServerPacket:
        receivePlayerMove(packetContents);
        break;

    case Packet::FromServerPacketContents::InitialPlayerDataFinishedFromServerPacket:
        m_initialPlayersReceivedFinished = true;
        break;

    case Packet::FromServerPacketContents::ChunkFromServerPacket:
        receiveChunk(packetContents);
        break;

    case Packet::QuickBarInventoryItemFromServerPacket:
        receiveQuickBarInventoryItem(packetContents);
        break;

    case Packet::QuickBarInventoryItemCountChangedFromServerPacket:
        receiveQuickBarInventoryItemCountChanged(packetContents);
        break;

    case Packet::ItemSpawnedFromServerPacket:
        receiveItemSpawned(packetContents);
        break;

    case Packet::WorldTimeChangedFromServerPacket:
        receiveWorldTimeChanged(packetContents);
        break;

    case Packet::InitialVegetationSpawningFromServerPacket:
        receiveInitialVegetationSpawning(packetContents);
        break;

    default:
        Debug::assertf(false, "Client failure, unhandled packet type received from server, it needs implemented apparently.");
    }

    enet_packet_destroy(event.packet);
}

void Client::receiveChatMessage(const std::string& packetContents)
{
    PacketBuf::ChatMessageFromServer chatMessage;
    Packet::deserialize(packetContents, &chatMessage);
//    m_chat->addChatLine(chatMessage.playername(), chatMessage.message());
}

void Client::receiveInitialPlayerData(const std::string& packetContents)
{
    PacketBuf::InitialPlayerDataFromServer message;
    Packet::deserialize(packetContents, &message);
    Debug::log(Debug::Area::NetworkClientInitialArea) << "initial player data received";

    Entities::Player* player = new Entities::Player("player1Standing1");
    std::stringstream chatMessage;
    if (!m_mainPlayer) {
        //this is must be *our* player, so create it
        m_mainPlayer = player;
        m_mainPlayer->setName(message.playername());
        m_mainPlayer->setPlayerID(message.playerid());
        m_mainPlayer->setPosition(message.x(), message.y());

        Debug::log(Debug::Area::NetworkClientInitialArea) << "initial player data received, pos: x: " << m_mainPlayer->position().x << " Y: " << m_mainPlayer->position().y;

        QuickBarInventory* quickBarInventory = new QuickBarInventory();
        m_mainPlayer->setQuickBarInventory(quickBarInventory);

        chatMessage << m_mainPlayer->name() << " has joined";
//        m_chat->addChatLine("", chatMessage.str());

        // this is us, the first player so this means the world creation is up to us
    Debug::log(Debug::Area::ImportantArea) << "CREATING WORLD< RECEIVED PLAYER DATA!";
        m_world = new World(m_mainPlayer, this, nullptr);

//        m_quickBarMenu = new QuickBarMenu(this, quickBarInventory, m_world->spriteSheetRenderer());
//        m_quickBarMenu->show();

    } else {
        player->setName(message.playername());
        player->setPlayerID(message.playerid());
        player->setPosition(message.x(), message.y());

        chatMessage << player->name() << " has joined";

        if (m_initialPlayersReceivedFinished) {
//            m_chat->addChatLine("", chatMessage.str());
        } else {
            Debug::log(Debug::Area::NetworkClientInitialArea) << "Player name: " << player->name() << " we're not adding the chat line because we haven't finished receiving initial client data";
        }
    }

    m_world->addPlayer(player);
}

void Client::receivePlayerDisconnected(const std::string& packetContents)
{
    PacketBuf::PlayerDisconnectedFromServer message;
    Packet::deserialize(packetContents, &message);

    Debug::assertf(m_world, "WARNING:, player disconnected, client attempted to remove him from the worlds player list..but we dont' have a world yet here? seems odd");
    m_world->findPlayer(message.playerid());
}

void Client::receivePlayerMove(const std::string& packetContents)
{
    PacketBuf::PlayerMoveFromServer message;
    Packet::deserialize(packetContents, &message);

    Entities::Player* player = m_world->findPlayer(message.playerid());
    player->setPosition(message.x(), message.y());
    Debug::log(Debug::Area::NetworkClientContinuousArea) << "Setting player position to: " << player->position().x << " Y: " << player->position().y;
}

void Client::receiveChunk(const std::string& packetContents)
{
    PacketBuf::Chunk message;
    Packet::deserialize(packetContents, &message);

    std::vector<Block> blocks;

    uint32_t index = 0;
    for (uint32_t row = message.starty(); row < message.endy(); ++row) {
        for (uint32_t column = message.startx(); column < message.endx(); ++column) {
            Block block;
            block.meshType = message.meshtype(index);
            block.primitiveType = message.primitivetype(index);
            block.wallType = message.walltype(index);
            blocks.push_back(block);

            ++index;
        }
    }

    Chunk chunk(message.startx(), message.starty(), message.endx(), message.endy(), &blocks);

    m_world->loadChunk(&chunk);
}

void Client::receiveWorldTimeChanged(const std::string& packetContents)
{
    PacketBuf::WorldTimeChangedFromServer message;
    Packet::deserialize(packetContents, &message);

    m_world->clientWorldTimeChanged(message.hour(), message.minute(), message.second());
}

void Client::receiveQuickBarInventoryItemCountChanged(const std::string& packetContents)
{
    PacketBuf::ItemCountChanged message;
    Packet::deserialize(packetContents, &message);

    if (message.newcount() == 0) {
//        m_quickBarMenu->inventory()->deleteItem(message.index());
    } else {
//        m_quickBarMenu->inventory()->item(message.index())->setStackSize(message.newcount());
    }

//    m_quickBarMenu->reloadSlot(message.index());
}

void Client::receiveQuickBarInventoryItem(const std::string& packetContents)
{
    PacketBuf::Item message;
    Packet::deserialize(packetContents, &message);

    //position isn't used even..but who cares.
    const glm::vec2 position = glm::vec2(message.x(), message.y());

    Item *baseItem = nullptr;
    switch (message.itemtype()) {
    case Item::ItemType::Block:
        break;

    case Item::ItemType::Torch: {
        Torch *torch = new Torch(position);
        torch->setRadius(message.radius());

        baseItem = torch;
        break;
    }

    case Item::ItemType::Tool: {
        Tool *tool = new Tool(position);
        tool->setToolMaterial(message.material());
        tool->setToolType(message.tooltype());

        baseItem = tool;
    }
    }

    Debug::assertf(baseItem, "client received a quickbar inventory item, but doesn't know how to generate it");

    //NOTE: we don't give a shit about position and such, the server doesn't even send that
    //in inventory scenarios, since it doesn't matter.
    baseItem->setStackSize(message.stacksize());
    baseItem->setName(message.itemname());
    baseItem->setDetails(message.itemdetails());
    baseItem->setState(message.itemstate());

    uint32_t index = message.index();

    //delete the old one as we'll get resent it in whatever new form it is in (whether it's moving to the world, or to a different inventory)
//    m_quickBarMenu->inventory()->deleteItem(index);

 //   m_quickBarMenu->inventory()->setSlot(index, baseItem);
  //  m_quickBarMenu->reloadSlot(index);
}

void Client::receiveItemSpawned(const std::string& packetContents)
{
    PacketBuf::Item message;
    Packet::deserialize(packetContents, &message);

    const glm::vec2 position = glm::vec2(message.x(), message.y());

    Item *baseItem = nullptr;
    switch (message.itemtype()) {
    case Item::ItemType::Block:
        break;

    case Item::ItemType::Torch: {
        Torch *torch = new Torch(position);
        torch->setRadius(message.radius());

        baseItem = torch;
        break;
    }
    }

    baseItem->setStackSize(message.stacksize());
    baseItem->setName(message.itemname());
    baseItem->setDetails(message.itemdetails());
    baseItem->setState(message.itemstate());

    m_world->spawnItem(baseItem);
}

void Client::receiveInitialVegetationSpawning(const std::string& packetContents)
{
    PacketBuf::InitialVegetationSpawn message;
    Packet::deserialize(packetContents, &message);

    assert(message.x_size() == message.y_size());

    for (int i = 0; i < message.x_size(); ++i) {
        Vegetation* tree = new Vegetation("tree1", SpriteSheetRenderer::SpriteSheetType::Entity);
        tree->setPosition(message.x(i), message.y(i));
        m_world->m_treesSpatialHash->insert(tree);
        m_world->m_spriteSheetRenderer->registerSprite(tree);
    }
}
