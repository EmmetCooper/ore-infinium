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
#include "debug.h"

#include <assert.h>

QuadTree::QuadTree(QuadTree* parent, b2Vec2 _center, b2Vec2 _halfDimension)
    : m_boundary(_center, _halfDimension),
      m_nodeCapacity(4),
      m_parent(parent)
{
    NW = NE = SW = SE = nullptr;
    m_points.reserve(m_nodeCapacity);
}

QuadTree::QuadTree(QuadTree* parent, b2Vec2 _center, b2Vec2 _halfDimension, int _nodeCapacity)
    : m_boundary(_center, _halfDimension),
      m_nodeCapacity(_nodeCapacity),
      m_parent(parent)
{
    NW = NE = SW = SE = nullptr;
    m_points.reserve(m_nodeCapacity);
}

bool QuadTree::insert(Entity * entity)
{
    Debug::assertf(entity, "quadtree insertion error, entity to insert was null (horrible api abuse)");
    if (!m_boundary.containsPoint(entity)) {
        return false;
    }

    if (m_points.size() < m_nodeCapacity) {
        m_points.push_back(entity);
        return true;
    }

    if (NW == nullptr) {
        subdivide();
    }

    if (NW->insert(entity)) {
        return true;
    }

    if (NE->insert(entity)) {
        return true;
    }

    if (SW->insert(entity)) {
        return true;
    }

    if (SE->insert(entity)) {
        return true;
    }

    assert(false);
    return false; // should never happen
}

void QuadTree::subdivide() {
    b2Vec2 center = m_boundary.center;
    b2Vec2 newDim(m_boundary.halfDimension.x / 2, m_boundary.halfDimension.y / 2);

    NW = new QuadTree(this, b2Vec2(center.x - newDim.x, center.y - newDim.y), newDim);
    NE = new QuadTree(this, b2Vec2(center.x + newDim.x, center.y - newDim.y), newDim);
    SW = new QuadTree(this, b2Vec2(center.x - newDim.x, center.y + newDim.y), newDim);
    SE = new QuadTree(this, b2Vec2(center.x + newDim.x, center.y + newDim.y), newDim);
}

void QuadTree::queryRange(std::vector<Entity*> & list, AABB range) {
    if (!m_boundary.intersectsAABB(range)) {
        return ; // list is empty
    }

    for (int i = 0; i < m_points.size(); ++i) {
        if (range.containsPoint(m_points[i])) {
            list.push_back(m_points[i]);
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
void QuadTree::clear() {
    if (this == nullptr) {
        return ;
    }

    m_points.clear();

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
