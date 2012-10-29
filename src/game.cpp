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
#include "imagemanager.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Config.hpp>

Game::Game()
{
}

void Game::abort_game(const char* message)
{
    printf("%s \n", message);
    shutdown();
    exit(1);
}

void Game::init()
{
    /*
    TODO:
    unsigned int VideoModesCount = sf::VideoMode::GetModesCount();
    for (unsigned int i = 0; i < VideoModesCount; ++i)
    {
        sf::VideoMode Mode = sf::VideoMode::GetMode(i);

        // Mode is a valid video mode
    }
    // Creating a fullscreen window with the best video mode supported
    App.Create(sf::VideoMode::GetMode(0), "SFML Window", sf::Style::Fullscreen);

    sf::VideoMode DesktopMode = sf::VideoMode::GetDesktopMode();
    */

    //TODO: debug only, on by default
    //    App.SetFramerateLimit(60); // Limit to 60 frames per second

    // sf::WindowSettings Settings = App.GetSettings();

    sf::WindowSettings settings;
    settings.depthBits = 24; // Request a 24 bits depth buffer
    settings.stencilBits = 8;  // Request a 8 bits stencil buffer
//    settings.AntialiasingLevel = 2;  // Request 2 levels of antialiasing

    m_app = new sf::RenderWindow(sf::VideoMode(SCREEN_W, SCREEN_H), "Buildarrhea", sf::Style::Resize, settings); //sf::Style::Fullscreen
    m_app->useVerticalSync(false);
    m_app->setFramerateLimit(0);

    m_view = new sf::View(sf::FloatRect(0, 0, SCREEN_W, SCREEN_H));
    m_app->setView(*m_view);

    m_font = new sf::Font();
    if (!m_font->LoadFromFile("../font/DejaVuSerif.ttf", 12))
    {
        abort_game("unable to load font");
    }

    ImageManager* manager = ImageManager::instance();
    manager->addResourceDir("../textures");

    m_player = new Player("../textures/player.png");

    tick();
    shutdown();
}

void Game::tick()
{
    sf::Event event;
    const sf::Input& Input = m_app->getInput();

    float Left = 0.f;
    float Top  = 0.f;

    m_player->setX(200.f);
    m_player->setY(100.f);
    m_player->setPosition(200.f, 100.f);
    m_player->setRotation(30.f);
    m_player->setCenter(0, 0);
    m_player->setScale(10.0f, 10.0f);
    m_player->setBlendMode(sf::Blend::Multiply);

    m_player->move(10, 5);
    m_player->rotate(90);

    if (!m_player->getImage()) {
        abort_game("no image");
    }

    sf::String text;
    text.setFont(*m_font);
    text.setSize(12.0);
    text.setColor(sf::Color::Yellow);

    std::stringstream ss;
    std::string str;

    //  "../textures/stone.png"
    while (m_app->isOpened())
    {
        float elapsedTime = m_app->getFrameTime();
        float fps = 1.f / elapsedTime;

        ss.str("");
        ss << "Framerate: " << fps;
        str = ss.str();
        text.SetText(str);

        while (m_app->getEvent(event))
        {

            bool LeftKeyDown = Input.isKeyDown(sf::Key::Left);
            bool RightButtonDown = Input.isMouseButtonDown(sf::Mouse::Right);
            unsigned int MouseX = Input.getMouseX();
            unsigned int MouseY = Input.getMouseY();

            // Move the sprite
            // if (m_app->GetInput().IsKeyDown(sf::Key::Left))  sprite.Move(-1000 * elapsedTime, 0);
            // if (m_app->GetInput().IsKeyDown(sf::Key::Right)) sprite.Move( 1000 * elapsedTime, 0);
            // if (m_app->GetInput().IsKeyDown(sf::Key::Up))    sprite.Move(0, -1000 * elapsedTime);
            // if (m_app->GetInput().IsKeyDown(sf::Key::Down))  sprite.Move(0,  1000 * elapsedTime);

            if (m_app->getInput().isKeyDown(sf::Key::Left))  m_view->move(-1000 * elapsedTime, 0);
            if (m_app->getInput().isKeyDown(sf::Key::Right)) m_view->move( 1000 * elapsedTime, 0);
            if (m_app->getInput().isKeyDown(sf::Key::Up))    m_view->move(0, -1000 * elapsedTime);
            if (m_app->getInput().isKeyDown(sf::Key::Down))  m_view->move(0,  1000 * elapsedTime);

            // Window closed
            if (event.type == sf::Event::Closed || ((event.Type == sf::Event::KeyPressed) && (event.Key.Code == sf::Key::Escape))) {
                shutdown();
            }
        }

        m_app->clear(sf::Color(200, 0, 0));
        m_app->draw(*m_player);

        m_app->setView(m_app->GetDefaultView());
        m_app->draw(text);
        m_app->setView(*m_view);

        // always after rendering!
        m_app->display();
    }
}

void Game::shutdown()
{
    m_app->close();
}
