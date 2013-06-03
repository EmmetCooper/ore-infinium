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

#include <chipmunk/chipmunk.h>

#include <vector>
#include <list>
#include <unordered_map>

//class SpatialHashRenderer;

class Sprite;

class SpatialHash
{
public:
    /**
     * @p cellSize the optimal cell size to divide this hash rectangle into. WARNING:
     * a non-optimal cellSize will yield poor performance. So it's best to be slightly bigger than the average
     * size of items it will contain.
     */
    SpatialHash(double x, double y, double width, double height, double cellSize, size_t reserve = 100);
    ~SpatialHash();

    void objectMoved(Sprite* object);
    void insert(Sprite* object);
    void remove(Sprite* object);
    void clear();

    std::vector<Sprite*> queryRange(double x, double y, double width, double height);

    struct Key {
        uint32_t x;
        uint32_t y;

        Key(uint32_t _x, uint32_t _y) : x(_x), y(_y)
        {
        }
    };

private:
    struct ObjectList {
       std::list<Sprite*> list;
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

    std::unordered_map<Key, ObjectList, KeyHash, KeyEqual> m_objects;

    double m_cellSize;
    double m_width;
    double m_height;
    double m_x;
    double m_y;

    friend class Sprite;
//    friend class SpatialHashRenderer;
};



#endif
