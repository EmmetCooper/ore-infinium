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

#include "src/network/packet.h"
#include "src/network/protobuf-compiled/packet.pb.h"

#include "src/fontmanager.h"
#include "gui/gui.h"
#include "gui/mainmenu.h"
#include "gui/chatdialog.h"

#include "src/settings/settings.h"

#include "src/world.h"
#include "src/debug.h"

#include <GL/glew.h>

Client::Client(const char* address, unsigned int port)
{
    m_client = enet_host_create (nullptr /* create a client host */,
                               1 /* only allow 1 outgoing connection */,
                               2 /* allow up 2 channels to be used, 0 and 1 */,
                               57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                               14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    Debug::assertf(m_client, "failed to create ENet client host");

    enet_address_set_host(&m_address, address);
    m_address.port = port;

    m_peer = enet_host_connect(m_client, &m_address, 2, 0);

    if (m_peer == NULL) {
        fprintf(stderr, "Client failed to connect to server");
        exit(EXIT_FAILURE);
    }

    m_font = FontManager::instance()->loadFont("../font/Ubuntu-L.ttf");
    m_font->FaceSize(12);

    m_gui = GUI::instance();
    m_mainMenu = new MainMenu(this);
    m_mainMenu->toggleShown();

    m_chat = new ChatDialog(this, m_mainMenu);
    m_chat->show();

    m_world = new World();
}

Client::~Client()
{
    enet_host_destroy(m_client);

    SDL_DestroyWindow(m_window);
    SDL_Quit();
}


void Client::initSDL()
{

    Debug::log(Debug::Area::System) << "SDL on platform: " << SDL_GetPlatform();

    SDL_version compiled;
    SDL_version linked;
    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);

    Debug::log(Debug::Area::System) << "Compiled against SDL version: " << int(compiled.major) << "." << int(compiled.minor) << "-" << int(compiled.patch) <<
    " Running (linked) against version: " << int(linked.major) << "." << int(linked.minor) << "-" << int(linked.patch);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        std::string error = SDL_GetError();
        Debug::fatal(false, Debug::Area::System, "failure to initialize SDL error: " + error);
    }

    m_window = SDL_CreateWindow("Ore Infinium", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                Settings::instance()->screenResolutionWidth, Settings::instance()->screenResolutionHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!m_window) {
        Debug::checkSDLError();
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Request opengl 3.3 context.
    // FIXME: i *want 3.2, but Mesa 9 only has 3.0.. :(
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //TODO: we'll probably need some extension at some point in time..
    //SDL_GL_ExtensionSupported();

    m_GLcontext = SDL_GL_CreateContext(m_window);
    Debug::checkGLError();

    SDL_ShowCursor(0);

    Debug::checkSDLError();
    Debug::assertf(glewInit() == GLEW_OK, "glewInit returned !GLEW_OK. No GL context can be formed..bailing out");

    Debug::log(Debug::Area::Graphics) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
    Debug::log(Debug::Area::Graphics) << "Platform: Renderer: " << glGetString(GL_RENDERER);
    Debug::log(Debug::Area::Graphics) << "OpenGL Version: " << glGetString(GL_VERSION);
    Debug::log(Debug::Area::Graphics) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    Debug::checkGLError();

    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::Graphics) << "Maximum OpenGL texture size allowed: " << textureSize;
    std::cout << "\n\n\n\n";

    if (!GLEW_KHR_debug) {
        Debug::log(Debug::Area::Graphics) << "GLEW_KHR_debug is not available, disabling OpenGL debug mode (TODO)";
        assert(0);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(&Debug::glDebugCallback, 0);
    glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE,0,0,GL_TRUE);

    Debug::fatal(enet_initialize != 0, Debug::Area::Network, "An error occurred during ENet init (network init failure");

    glClearColor(.5f, 0.f, 0.f, 1.0f);

    glViewport(0, 0, Settings::instance()->screenResolutionWidth, Settings::instance()->screenResolutionHeight);

    Debug::checkGLError();
}

void Client::poll()
{
    ENetEvent event;
    int eventStatus;

    eventStatus = 1;

    eventStatus = enet_host_service(m_client, &event, 0);

    // If we had some event that interested us
    if (eventStatus > 0) {
        switch(event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("(Client) We got a new connection from %x\n", event.peer->address.host);

                char hostname[32];
                enet_address_get_host_ip(&event.peer->address, hostname, static_cast<size_t>(32));
                printf("(Client) CONNECTED TO HOST IP: %s\n", hostname);
//                Debug::log()
                enet_peer_ping(event.peer);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
//                printf("(Client) Message from server : %s\n", event.packet->data);
                std::cout << "(Client) Message from server, our client->server round trip latency is: " << event.peer->lastRoundTripTime << "\n";


                // Lets broadcast this message to all
                // enet_host_broadcast(client, 0, event.packet);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("(Client) %s disconnected.\n", event.peer->data);
                // Reset client's information
                event.peer->data = NULL;
                break;
        }
    }

    std::stringstream ss(std::stringstream::out | std::stringstream::binary);

    PacketBuf::ChatMessage message;
    message.set_message("THIS IS A TEST PROTOBUF (FUCK YEAH) MESSAGE FROM CLIENT");

    Packet::serialize(&ss, &message, Packet::FromClientPacketContents::ChatMessageFromClientPacket);

    ENetPacket *packet = enet_packet_create(ss.str().c_str(), ss.str().size(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(m_peer, 0, packet);
}

double fps = 0.0;

void Client::render(double elapsedTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_world->render();

    m_gui->render();
    drawDebugText(elapsedTime);

    SDL_GL_SwapWindow(m_window);
}

void Client::tick(double elapsedTime)
{
    handleInputEvents();
    m_world->update(elapsedTime);
}

void Client::drawDebugText(double frametime)
{
    std::stringstream ss;
    std::string str;

    ss.str("");
    ss << "FPS: " << fps;
    ss << " Frametime: " << frametime;
    str = ss.str();

    const int height = Settings::instance()->screenResolutionHeight - 15;
    const int width = Settings::instance()->screenResolutionWidth;

    m_font->Render(str.c_str(), -1, FTPoint(0.0, height - 0.0, 0.0));
    m_font->Render("F5 to toggle debug logging", -1, FTPoint(0.0, height - 15.0, 0.0));
    m_font->Render("F6 to toggle renderer logging", -1, FTPoint(0.0, height - 30.0, 0.0));
    m_font->Render("F7 to toggle GUI renderer debug", -1, FTPoint(0.0, height - 45.0, 0.0));
}

void Client::handleInputEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        m_gui->handleEvent(event);

        if (!m_gui->inputDemanded()) {
            m_world->handleEvent(event);
        }

        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_mainMenu->toggleShown();
                } else if (event.key.keysym.sym == SDLK_F5) {
                    // toggle debug logging
                    Settings::instance()->debugOutput = !Settings::instance()->debugOutput;
                } else if (event.key.keysym.sym == SDLK_F6) {
                    Settings::instance()->debugRendererOutput = !Settings::instance()->debugRendererOutput;
                } else if (event.key.keysym.sym == SDLK_F7) {
                    // toggle debug rendering
                    Settings::instance()->debugGUIRenderingEnabled = !Settings::instance()->debugGUIRenderingEnabled;
                    m_gui->debugRenderingChanged();
                }
                break;

            case SDL_WINDOWEVENT_CLOSE:
                m_mainMenu->toggleShown();
                break;

            case SDL_QUIT:
                shutdown();
                break;

            default:
                break;
        }
    }
}

void Client::shutdown()
{

}
