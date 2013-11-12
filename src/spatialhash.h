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

#ifndef SPATIALHASH_H
#define SPATIALHASH_H

#include <vector>
#include <list>
#include <unordered_map>

#include <QSet>

//FIXME:class SpatialHashRenderer;

class Sprite;
class Object;

class SpatialHash
{
public:
    enum class SpatialHashContents {
        SpriteSpatialHashContents,
        ObjectSpatialHashContents
    };

    /**
     * @p cellSize the optimal cell size to divide this hash rectangle into. WARNING:
     * a non-optimal cellSize will yield poor performance. So it's best to be slightly bigger than the average
     * size of items it will contain.
     *
     * NOTE: you should specify a reserve size, as it has no idea what you want to store and is just set at some small number.
     *
     */
    SpatialHash(double x, double y, double width, double height, double cellSize, size_t reserve = 100);

    /**
     * If @p contents is not specified, uses a different constructor and is assumed to be holding sprites, as that's the most common.
     * @sa SpatialHashContents
     * WARNING: cellSize must be exactly as big as the contents. Though size is not taken into account when it comes to querying, since
     * it is meant for fluids only and other fixed-size content.
     * Therefore, do not attempt to store any non-objects in here at the same time as sprites. Sprites take size into account, objects do not.
     */
    SpatialHash(double x, double y, double width, double height, double cellSize, SpatialHashContents contents, size_t reserve = 100);
    ~SpatialHash();

    void objectMoved(Sprite* object);
    void insert(Sprite* object);
    void remove(Sprite* object);

    void objectMoved(Object* object);
    void insert(Object* object);
    void remove(Object* object);

    void clear();

    void queryRange(QSet<Sprite*> *results, double x, double y, double x2, double y2);

    /**
     * Different from traditional @sa queryRange
     * calls, because this one is ONLY VALID for objects which cannot possibly span more than one cell per object.
     * (Which avoids the overhead of using a set).
     */
    void queryRangeNonOverlapping(std::vector<Object*> *results, double x, double y, double x2, double y2);

    struct Key {
        uint32_t x;
        uint32_t y;

        Key(uint32_t _x, uint32_t _y) : x(_x), y(_y)
        {
        }
    };

private:
    struct SpriteList {
       std::list<Sprite*> list;
       //TODO: is there something else I need in here? I feel like there could be..
    };

    struct ObjectList {
       std::list<Object*> list;
       //TODO: is there something else I need in here? I feel like there could be..
    };

    struct KeyHash {
        std::size_t operator()(const Key& k) const
        {
            std::hash<int> h;
            return h(k.x) ^ (h(k.y) << 1);
        }
    };

    struct KeyEqual {
        bool operator()(const Key& lhs, const Key& rhs) const
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };

    std::unordered_map<Key, SpriteList, KeyHash, KeyEqual> m_spriteObjects;
    std::unordered_map<Key, ObjectList, KeyHash, KeyEqual> m_objects;

    SpatialHashContents m_contents = SpatialHashContents::SpriteSpatialHashContents;

    double m_cellSize;
    double m_x2;
    double m_y2;
    double m_x;
    double m_y;

//    friend class SpatialHashRenderer;
};



#endif
