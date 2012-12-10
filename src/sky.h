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

#ifndef SKY_H
#define SKY_H

#include "renderable.h"

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderStates.hpp>

class CloudSystem;

class Sky
{
public:
    Sky(sf::RenderWindow *window, sf::View *view, float height);

    void update();
    void render();

private:
    sf::RenderWindow *m_window;
    sf::View *m_view;

    Renderable m_sunSprite;
    Renderable m_moonSprite;
    Renderable m_skyBox;

    sf::Image m_skyBoxImage;

    sf::Vector2f m_sunPosition;
    sf::Vector2f m_moonPosition;

    CloudSystem *m_cloudSystem = nullptr;

    float m_timeAngle = 0.0f;
    int m_hour = 0;

    // the height of the sky
    float m_height;
};

#endif
