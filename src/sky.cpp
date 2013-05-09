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

#include "sky.h"

#include "src/world.h"
#include "src/time.h"

#include <iostream>
#include <stdio.h>

#include <math.h>

Sky::Sky(World* world)
{

}

void Sky::update(const float elapsedTime)
{
//    sf::Vector2f _viewportCenter;
//    _viewportCenter.x = SCREEN_W / 2;
//    _viewportCenter.y = SCREEN_H / 2;
//
//    const unsigned char hour = Time::instance()->currentHour();
//    const unsigned char minute = Time::instance()->currentMinute();
//    std::cout << "MINUTE! " << (short) minute << "\n";
//
//    m_timeAngle = (hour * (180 / 12)) + (minute * (180 / 12) / 60 );
//
//    double angle = (m_timeAngle + 90) * (M_PI / 180);
//    float newX = _viewportCenter.x + cos(angle) * 400;
//    float newY = _viewportCenter.y + sin(angle) * 400;
//    m_sunPosition = sf::Vector2f(newX, newY);
//    m_sunSprite.setPosition(m_sunPosition);
//
//    angle = (m_timeAngle + 90 - 180) * (M_PI / 180);
//    newX = _viewportCenter.x + cos(angle) * 400;
//    newY = _viewportCenter.y + sin(angle) * 400;
//
//    m_moonPosition = sf::Vector2f(newX, newY);
//    m_moonSprite.setPosition(m_moonPosition);
}

void Sky::render()
{
 //   sf::VertexArray line(sf::Lines, 2);
 //   sf::Vector2f screen = sf::Vector2f(SCREEN_W/2, SCREEN_H/2);
 //   line.append(sf::Vertex(screen));
 //   line.append(sf::Vertex(m_sunPosition));
 //   m_window->draw(line);
 //
 //   // sunrise at 7 am, sunset at 7am..
 //   const unsigned char hour = Time::instance()->currentHour();
 //
 //   // 7am, 7pm
 //   if (hour >= 7 && hour < 19) {
 //       m_window->draw(m_sunSprite);
 //       m_sunSprite.render(m_window);
 //   } else {
 //       m_window->draw(m_moonSprite);
 //       m_moonSprite.render(m_window);
 //   }
}
