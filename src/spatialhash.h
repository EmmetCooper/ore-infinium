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

#include "src/entity.h"

#include <chipmunk/chipmunk.h>

#include <unordered_map>

//class SpatialHashRenderer;

class SpatialHash
{
public:
    /**
     * @p cellSize the optimal cell size to divide this hash rectangle into. WARNING:
     * a non-optimal cellSize will yield poor performance. So it's best to be slightly bigger than the average
     * size of items it will contain.
     */
    SpatialHash(double x, double y, double width, double height, double cellSize);
    ~SpatialHash();

    void insert(Entity* entity);
    void clear();

private:
    struct EntityList {
       std::vector<Entity*> list;
       //TODO: is there something else I need in here? I feel like there could be..
    };

    struct Key {
        int x;
        int y;
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

    std::unordered_map<Key, std::string, KeyHash, KeyEqual> map;
//    friend class SpatialHashRenderer;
};



#endif
