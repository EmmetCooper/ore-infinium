/******************************************************************************
 *   Copyright (C) 2012, 2013 by Shaun Reich <sreich@kde.org>                 *
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

#ifndef OBJECT_H
#define OBJECT_H

#include "spatialhash.h"

#include "glm/glm.hpp"

class SpatialHash;

class Object
{
public:
    explicit Object();

    Object(const Object& sprite);

    virtual ~Object();

    void setPosition(const glm::vec2& vector) {
        m_position = vector;
    }

    void setPosition(float x, float y) {
        m_position = glm::vec2(x, y);

        if (m_spatialHash) {
            m_spatialHash->objectMoved(this);
        }
    }

    glm::vec2 position() const {
        return m_position;
    }

private:
    SpatialHash* m_spatialHash = nullptr;

    // position is in meters as is everything else the game deals with. size is in meters as well, converted from pixels.
    glm::vec2 m_position;

    /// utilized only by spatial index for speedy lookups (removals as well as findings)
    std::vector<SpatialHash::Key> m_spatialHashKeys;

    friend class SpatialHash;
};

#endif
