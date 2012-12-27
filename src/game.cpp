/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#include "game.h"

#include "debug.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <assert.h>

Game::Game()
{
}

Game::~Game()
{
}

void Game::abort_game(const char* message)
{
    printf("%s \n", message);
    shutdown();
    exit(1);
}

void checkSDLError()
{
    std::string error = SDL_GetError();
    if (*error.c_str() != '\0')
    {
        Debug::log(Debug::Area::System) << "SDL Error: " << error;
        SDL_ClearError();
    }
}

void Game::init()
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

    m_window = SDL_CreateWindow("Ore Chasm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (!m_window) {
        checkSDLError();
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    /* Request opengl 3.2 context.
     * SDL doesn't have the ability to choose which profile at this time of writing,
     * but it should default to the core profile */
    //FIXME: i *want 3.2, but Mesa 9 only has 3.0.. :(
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    /* Turn on double buffering with a 24bit Z buffer.
     * You may need to change this to 16 or 32 for your system */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    //TODO: we'll probably need some extension at some point in time..
    //SDL_GL_ExtensionSupported();

    m_context = SDL_GL_CreateContext(m_window);

    checkSDLError();

    Debug::log(Debug::Area::Graphics) << "Platform: Driver Vendor: " << glGetString(GL_VENDOR);
    Debug::log(Debug::Area::Graphics) << "Platform: Renderer: " << glGetString(GL_RENDERER);
    Debug::log(Debug::Area::Graphics) << "OpenGL Version: " << glGetString(GL_VERSION);
    Debug::log(Debug::Area::Graphics) << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    GLint textureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &textureSize);
    Debug::log(Debug::Area::Graphics) << "Maximum OpenGL texture size allowed: " << textureSize;
    std::cout << "\n\n\n\n";

    m_font = new FTGLPixmapFont("../font/Ubuntu-L.ttf");
    Debug::fatal(!m_font->Error(), Debug::Area::System, "Failure to load font");

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    glViewport(0, 0, SCREEN_W, SCREEN_H);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, SCREEN_W, SCREEN_H, 0, 1, -1);

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);

    glLoadIdentity();

//    ImageManager* manager = ImageManager::instance();
//    manager->addResourceDir("../textures/");

    //World::createInstance(m_display);
    //m_world = World::instance();
    m_font->FaceSize(12);

    tick();
    shutdown();
}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    m_running = false;
                }
                break;

            case SDL_WINDOWEVENT_CLOSE:
                m_running = false;
                break;

            case SDL_QUIT:
                exit(0);
                break;

            default:
                break;
        }
    }
}

void Game::render()
{
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
    glColor3f(1, 1, 0); glVertex3f(100, 0, 0);
    glColor3f(1, 0, 1); glVertex3f(100, 100, 0);
    glColor3f(1, 1, 1); glVertex3f(0, 100, 0);
    glEnd();
}

double fps = 0.0;

void Game::tick()
{
    Uint32 startTime = SDL_GetTicks();
    int frameCount = 0;

    while (m_running) {
        fps =(frameCount / float(SDL_GetTicks() - startTime)) * 1000;

           // m_world->update(static_cast<float>(delta));
           // m_world->render();

            //rendering always before this

            handleEvents();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            render();
            //render some crap
            drawDebugText();

            SDL_GL_SwapWindow(m_window);

            ++frameCount;
    }

shutdown:
    shutdown();
}

void Game::drawDebugText()
{
    std::stringstream ss;
    std::string str;

    ss.str("");
    ss << "FPS: " << fps;
    str = ss.str();
    m_font->Render(str.c_str());
}

void Game::shutdown()
{
    SDL_Quit();
    exit(0);
}
