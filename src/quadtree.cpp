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

#include <chipmunk/chipmunk.h>

#include <assert.h>

QuadTree::QuadTree(QuadTree* parent, cpBB bb)
    : m_boundary(bb),
      m_nodeCapacity(4),
      m_parent(parent)
{
    m_points.reserve(m_nodeCapacity);
}

QuadTree::QuadTree(QuadTree* parent, cpBB bb, size_t _nodeCapacity)
    : m_boundary(bb),
      m_nodeCapacity(_nodeCapacity),
      m_parent(parent)
{
    m_points.reserve(m_nodeCapacity);
}

bool QuadTree::insert(Entity * entity)
{
    Debug::assertf(entity, "quadtree insertion error, entity to insert was null (horrible api abuse)");
    if (!cpBBContainsVect(m_boundary, cpv(entity->position().x, entity->position().y))) {
//        Debug::log(Debug::ImportantArea) << "enttity position x: " << entity->position().x << " y: " << entity->position().y << " bounding box does not contain it. BB left: " << m_boundary.l << " right: " << m_boundary.r << " bottom: " << m_boundary.b << " top: " << m_boundary.t;
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
    // create four children which fully divide this quad into four quads of equal area
    cpVect quarterSize = cpv(m_boundary.r * 0.25, m_boundary.b * 0.25);
    NW = new QuadTree(this, cpBBNew(m_boundary.l, m_boundary.t + quarterSize.y, m_boundary.l + quarterSize.x, m_boundary.t));
    NE = new QuadTree(this, cpBBNew(m_boundary.l + quarterSize.x, m_boundary.t + quarterSize.y, m_boundary.r - quarterSize.x, m_boundary.t));
    SW = new QuadTree(this, cpBBNew(m_boundary.l, m_boundary.b, m_boundary.l + quarterSize.x, m_boundary.b - quarterSize.y));
    SE = new QuadTree(this, cpBBNew(m_boundary.l + quarterSize.x, m_boundary.b, m_boundary.r, m_boundary.t + quarterSize.y));
}

void QuadTree::queryRange(std::vector<Entity*>* list, cpBB range) {
    if (!cpBBIntersects(range, m_boundary)) {
        Debug::log(Debug::Area::ImportantArea) << "INTERSECTION FAILURE";
       return; //return unchanged (empty) list.
    }

        Debug::log(Debug::Area::ImportantArea) << "INTERSECTION SUCCASS";

    if (NW == nullptr) {
        return ;
    }

    for (size_t i = 0; i < m_points.size(); ++i) {
       const glm::vec2& position = m_points[i]->position();
        if (cpBBContainsVect(m_boundary, cpv(position.x, position.y))) {
            list->push_back(m_points[i]);
        }
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
