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

#ifndef SKY_H
#define SKY_H

#include <glm/glm.hpp>

class World;

class Sky
{
public:
    Sky(World* world);

    void update(const float elapsedTime);
    void render();

private:

    //Renderable m_sunSprite;
    //Renderable m_moonSprite;
    //Renderable m_skyBox;

    //sf::Texture m_skyBoxDayTexture;
    //sf::Texture m_skyBoxDuskTexture;
    //sf::Texture m_skyBoxNightTexture;

    glm::vec2 m_sunPosition;
    glm::vec2 m_moonPosition;

    //CloudSystem *m_cloudSystem = nullptr;

    float m_timeAngle = 0.0f;
    int m_hour = 0;
};

#endif
