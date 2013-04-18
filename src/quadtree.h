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

#ifndef __QUADTREE__
#define __QUADTREE__

#include "src/entity.h"

#include <Box2D/Box2D.h>

struct AABB {
    AABB(b2Vec2 _center, b2Vec2 _halfDimension) : center(_center), halfDimension(_halfDimension) {}

    bool containsPoint(Entity* entity) {
        b2Vec2 p(entity->position().x, entity->position().y);
        return ((p.x > center.x - halfDimension.x && p.x <= center.x + halfDimension.x) &&
                (p.y > center.y - halfDimension.y && p.y <= center.y + halfDimension.y));
    }

    bool intersectsAABB(AABB other) {
        return (((other.center.x - other.halfDimension.x > center.x - halfDimension.x && other.center.x - other.halfDimension.x < center.x + halfDimension.x) ||
                 (other.center.x + other.halfDimension.x > center.x - halfDimension.x && other.center.x + other.halfDimension.x < center.x + halfDimension.x)) &&
                ((other.center.y - other.halfDimension.y > center.y - halfDimension.y && other.center.y - other.halfDimension.y < center.y + halfDimension.y) ||
                 (other.center.y + other.halfDimension.y > center.y - halfDimension.y && other.center.y + other.halfDimension.y < center.y + halfDimension.y)));
    }

    b2Vec2 center;
    b2Vec2 halfDimension;
};

class Quadtree {
public:
    Quadtree(b2Vec2, b2Vec2);
    Quadtree(b2Vec2, b2Vec2, int);

    bool insert(Entity *);
    void subdivide();
    void queryRange(std::vector<Entity*> &, AABB);

    void clear();
    bool getCollision(std::vector<Entity*>);

    AABB boundary;
    int nodeCapacity;

    // leaves
    Quadtree * NW;
    Quadtree * NE;
    Quadtree * SW;
    Quadtree * SE;

    // data
    std::vector<Entity*> points;
};

#endif // __QUADTREE__
