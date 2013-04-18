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

#include "quadtree.h"

#include <assert.h>

Quadtree::Quadtree(b2Vec2 _center, b2Vec2 _halfDimension)
    : boundary(_center, _halfDimension),
      nodeCapacity(4)
{
    NW = NE = SW = SE = nullptr;
    points.reserve(nodeCapacity);
}

Quadtree::Quadtree(b2Vec2 _center, b2Vec2 _halfDimension, int _nodeCapacity)
    : boundary(_center, _halfDimension),
      nodeCapacity(_nodeCapacity)
{
    NW = NE = SW = SE = nullptr;
    points.reserve(nodeCapacity);
}

bool Quadtree::insert(Entity * a)
{
    if (!boundary.containsPoint(a)) {
        return false;
    }

    if (points.size() < nodeCapacity) {
        points.push_back(a);
        return true;
    }

    if (NW == nullptr) {
        subdivide();
    }

    if (NW->insert(a)) {
        return true;
    }

    if (NE->insert(a)) {
        return true;
    }

    if (SW->insert(a)) {
        return true;
    }

    if (SE->insert(a)) {
        return true;
    }

    assert(false);
    return false; // should never happen
}

void Quadtree::subdivide() {
    b2Vec2 center = boundary.center;
    b2Vec2 newDim(boundary.halfDimension.x / 2, boundary.halfDimension.y / 2);

    NW = new Quadtree(b2Vec2(center.x - newDim.x, center.y - newDim.y), newDim);
    NE = new Quadtree(b2Vec2(center.x + newDim.x, center.y - newDim.y), newDim);
    SW = new Quadtree(b2Vec2(center.x - newDim.x, center.y + newDim.y), newDim);
    SE = new Quadtree(b2Vec2(center.x + newDim.x, center.y + newDim.y), newDim);
}

void Quadtree::queryRange(std::vector<Entity*> & list, AABB range) {
    if (!boundary.intersectsAABB(range)) {
        return ; // list is empty
    }

    for (int i = 0; i < points.size(); ++i) {
        if (range.containsPoint(points[i])) {
            list.push_back(points[i]);
        }
    }

    if (NW == nullptr) {
        return ;
    }

    NW->queryRange(list, range);
    NE->queryRange(list, range);
    SW->queryRange(list, range);
    SE->queryRange(list, range);
}

// scan the tree and remove all node/Item*
void Quadtree::clear() {
    if (this == nullptr) {
        return ;
    }

    points.clear();

    NW->clear();
    delete NW;
    NW = nullptr;

    NE->clear();
    delete NE;
    NE = nullptr;

    SW->clear();
    delete SW;
    SW = nullptr;

    SE->clear();
    delete SE;
    SE = nullptr;
}
