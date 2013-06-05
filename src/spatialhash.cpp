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

#include "spatialhash.h"

#include "src/entity.h"
#include "debug.h"

#include <chipmunk/chipmunk.h>

#include <assert.h>

SpatialHash::SpatialHash(double x, double y, double x2, double y2, double cellSize, size_t reserve) :
    m_cellSize(cellSize),
    m_x2(x2),
    m_y2(y2),
    m_x(x),
    m_y(y)
{
    assert(x2 > x && y2 > y && (cellSize > 0.0));
    m_objects.reserve(reserve);
}

SpatialHash::~SpatialHash()
{

}

void SpatialHash::insert(Sprite* object)
{
    assert(object);
    object->m_spatialHash = this;

    const glm::vec2& position = object->position();
    assert(position.x >= m_x && position.x <= m_x2 && position.y >= m_y && position.y <= m_y2);

    uint32_t cellX = position.x / m_cellSize;
    uint32_t cellY = position.y / m_cellSize;
    Key* key = new Key(cellX, cellY);

    m_objects[*key].list.push_back(object);

    object->m_spatialHashKey = key;
}

void SpatialHash::remove(Sprite* object)
{
    assert(object);

    object->m_spatialHash = nullptr;
    Key* key = object->m_spatialHashKey;

    SpatialHash::ObjectList& value = m_objects.at(*key);
    value.list.remove(object);

    delete key;
    object->m_spatialHashKey = nullptr;
}

void SpatialHash::objectMoved(Sprite* object)
{
    assert(object);

    if (object->m_spatialHashKey == nullptr) {
       return;
    }

    /*
    const glm::vec2& position = entity->position();
    assert(position.x >= m_x && position.x <= m_width && position.y >= m_y && position.y <= m_height);

    uint32_t cellX = position.x / m_cellSize;
    uint32_t cellY = position.y / m_cellSize;
    Key key(cellX, cellY);

    */
}

void SpatialHash::clear()
{

}

void SpatialHash::queryRange(std::vector<Sprite*> *results, double x, double y, double x2, double y2)
{
    uint32_t startX = x / m_cellSize;
    uint32_t startY = y / m_cellSize;

    uint32_t endX = x2 / m_cellSize;
    uint32_t endY = y2 / m_cellSize;

    for (uint32_t y = startY; y <= endY; y += 1) {
        for (uint32_t x = startX; x <= endX; x += 1) {
            Key key(x, y);

            auto it = m_objects.find(key);

            if (it != m_objects.end()) {
                for (auto* object : it->second.list) {
                    results->push_back(object);
                }
            }
        }
    }
}
