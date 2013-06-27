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
#include "src/object.h"
#include "src/sprite.h"

#include "debug.h"

#include <chipmunk/chipmunk.h>

#include <set>
#include <assert.h>

SpatialHash::SpatialHash(double x, double y, double x2, double y2, double cellSize, size_t reserve) :
    m_contents(SpatialHashContents::SpriteSpatialHashContents),
    m_cellSize(cellSize),
    m_x2(x2),
    m_y2(y2),
    m_x(x),
    m_y(y)
{
    assert(x2 > x && y2 > y && (cellSize > 0.0));
    m_spriteObjects.reserve(reserve);
}

SpatialHash::SpatialHash(double x, double y, double x2, double y2, double cellSize, SpatialHashContents contents, size_t reserve) :
    m_contents(contents),
    m_cellSize(cellSize),
    m_x2(x2),
    m_y2(y2),
    m_x(x),
    m_y(y)
{
    assert(x2 > x && y2 > y && (cellSize > 0.0));

    switch (m_contents) {
        case SpatialHashContents::SpriteSpatialHashContents: {
        m_spriteObjects.reserve(reserve);
        break;
        }

        case SpatialHashContents::ObjectSpatialHashContents: {
        m_objects.reserve(reserve);
        break;
        }

    }

}

SpatialHash::~SpatialHash()
{

}

void SpatialHash::insert(Sprite* object)
{
    assert(m_contents == SpatialHashContents::SpriteSpatialHashContents);
    assert(object);
    object->m_spatialHash = this;

    const glm::vec2& position = object->position();
    const glm::vec2& size = object->sizeMeters();

    // verify that the object at least resides within the bounds of this spatial hash...
    assert(position.x >= m_x && position.x <= m_x2 && position.y >= m_y && position.y <= m_y2);

    uint32_t startX = position.x / m_cellSize;
    uint32_t startY = position.y / m_cellSize;
    uint32_t endX = (position.x + size.x) / m_cellSize;
    uint32_t endY = (position.y + size.y) / m_cellSize;

    for (uint32_t cellY = startY; cellY <= endY; cellY += 1) {
        for (uint32_t cellX = startX; cellX <= endX; cellX += 1) {

            Key key(cellX, cellY);

            m_spriteObjects[key].list.push_back(object);

            object->m_spatialHashKeys.push_back(key);
        }
    }
}

void SpatialHash::remove(Sprite* object)
{
    assert(m_contents == SpatialHashContents::SpriteSpatialHashContents);
    assert(object);

    object->m_spatialHash = nullptr;

    for (auto& key : object->m_spatialHashKeys) {
        SpatialHash::SpriteList& value = m_spriteObjects.at(key);
        value.list.remove(object);
    }

    object->m_spatialHashKeys.clear();
}

//FIXME: unimplemented
void SpatialHash::objectMoved(Sprite* object)
{
    assert(m_contents == SpatialHashContents::SpriteSpatialHashContents);
    assert(object);

    if (object->m_spatialHashKeys.size() == 0) {
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

void SpatialHash::queryRange(std::set<Sprite*> *results, double x, double y, double x2, double y2)
{
    assert(m_contents == SpatialHashContents::SpriteSpatialHashContents);

    uint32_t startX = x / m_cellSize;
    uint32_t startY = y / m_cellSize;

    uint32_t endX = x2 / m_cellSize;
    uint32_t endY = y2 / m_cellSize;

    cpBB queryBB = cpBBNew(x, y, x2, y2);

    for (uint32_t y = startY; y <= endY; ++y) {
        for (uint32_t x = startX; x <= endX; ++x) {
            Key key(x, y);

            // if the cell exists (and therefore more likely to have something in it)
            auto it = m_spriteObjects.find(key);

            if (it != m_spriteObjects.end()) {
                for (auto* object : it->second.list) {

                    const glm::vec2& position = object->position();
                    const glm::vec2& size = object->sizeMeters();
                    cpBB objectBB = cpBBNew(position.x, position.y, position.x + size.x, position.y + size.y);

                    if (cpBBIntersects(queryBB, objectBB)) {
                        results->insert(object);
                    }
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////// similar code, but for Objects instead..I'd prefer these to be merged of course, though not a big deal. Used for "lightweight" things like Fluids which don't need the other shit that Sprite hauls along

void SpatialHash::queryRange(std::set<Object*>* results, double x, double y, double x2, double y2)
{
    assert(m_contents == SpatialHashContents::ObjectSpatialHashContents);

    uint32_t startX = x / m_cellSize;
    uint32_t startY = y / m_cellSize;

    uint32_t endX = x2 / m_cellSize;
    uint32_t endY = y2 / m_cellSize;

    for (uint32_t y = startY; y <= endY; ++y) {
        for (uint32_t x = startX; x <= endX; ++x) {
            Key key(x, y);

            // if the cell exists (and therefore more likely to have something in it)
            auto it = m_objects.find(key);

            if (it != m_objects.end()) {
                for (auto* object : it->second.list) {

                    const glm::vec2& position = object->position();
                    results->insert(object);
                }
            }
        }
    }

}

void SpatialHash::insert(Object* object)
{
    assert(m_contents == SpatialHashContents::ObjectSpatialHashContents);
    assert(object);

    object->m_spatialHash = this;

    const glm::vec2& position = object->position();

    // verify that the object at least resides within the bounds of this spatial hash...
    assert(position.x >= m_x && position.x <= m_x2 && position.y >= m_y && position.y <= m_y2);

    uint32_t cellX = position.x / m_cellSize;
    uint32_t cellY = position.y / m_cellSize;

    Key key(cellX, cellY);

    m_objects[key].list.push_back(object);

    object->m_spatialHashKeys.push_back(key);
}

void SpatialHash::remove(Object* object)
{
    assert(m_contents == SpatialHashContents::ObjectSpatialHashContents);
    assert(object);

    object->m_spatialHash = nullptr;

    for (auto& key : object->m_spatialHashKeys) {
        SpatialHash::ObjectList& value = m_objects.at(key);
        value.list.remove(object);
    }

    object->m_spatialHashKeys.clear();
}

void SpatialHash::objectMoved(Object* object)
{
    assert(m_contents == SpatialHashContents::ObjectSpatialHashContents);
    assert(object);

    assert(0); //FIXME: unimplemented
}

