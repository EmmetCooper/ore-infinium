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

QuadTree::QuadTree() :
    m_left(0),
    m_right(0),
    m_top(0),
    m_bottom(0),
    m_numObjectsToGrow(4),
    m_nodes(0),
    m_isLeaf(true),
    m_parent(nullptr)
{
}

QuadTree::QuadTree(double left, double right, double top, double bottom, QuadTree* parent, uint32_t _numObjectsToGrow) :
    m_left(left),
    m_right(right),
    m_top(top),
    m_bottom(bottom),
    m_numObjectsToGrow(_numObjectsToGrow),
    m_nodes(0),
    m_isLeaf(true),
    m_parent(parent)
{
}

QuadTree::~QuadTree()
{
    if (!m_isLeaf) {
        delete [] m_nodes;
    }
}

void QuadTree::insert(Entity* entity)
{
    if (m_isLeaf) {
        m_entities.push_back(entity);

        bool reachedMaxObjects = (m_entities.size() == m_numObjectsToGrow);

        if (reachedMaxObjects) {
            createLeaves();
            moveObjectsToLeaves();
            m_isLeaf = false;
        }
        return;
    }

    for (int n = 0; n < NodeCount; ++n) {
        if (m_nodes[n].contains(entity)) {
            m_nodes[n].insert(entity);
            return;
        }
    }

    m_entities.push_back(entity);
}

void QuadTree::clear()
{
    m_entities.clear();

    if (!m_isLeaf) {
        for (int n = 0; n < NodeCount; ++n) {
            m_nodes[n].clear();
        }
    }
}

std::vector<Entity*> QuadTree::queryRange(double x, double y, double width, double height)
{
    if (m_isLeaf) {
        return m_entities;
    }

    std::vector<Entity*> returnedObjects;
    std::vector<Entity*> childObjects;

    if (!m_entities.empty()) {
        returnedObjects.insert(returnedObjects.end(), m_entities.begin(), m_entities.end());
    }

    for (int n = 0; n < NodeCount; ++n) {
        if (m_nodes[n].containsRect(x, y, width, height)) {
            childObjects = m_nodes[n].queryRange(x, y, width, height);
            returnedObjects.insert(returnedObjects.end(), childObjects.begin(), childObjects.end());
            break;
        }
    }

    return returnedObjects;
}

bool QuadTree::contains(Entity* entity)
{
    const glm::vec2& position = entity->position();
    const glm::vec2& size = entity->sizeMeters();

    return  position.x > m_left &&
            (position.x + size.x) < m_right &&
            position.y > m_top &&
            (position.y + size.y) < m_bottom;
}

bool QuadTree::containsRect(double x, double y, double width, double height)
{
    return (x >= m_left && x <= m_right) &&
           (y >= m_top && y <= m_bottom) &&
           (width <= m_right && height <= m_bottom);
}

void QuadTree::createLeaves()
{
    m_nodes = new QuadTree[4];
    m_nodes[NW] = QuadTree(m_left, (m_left + m_right) / 2.0, m_top, (m_top + m_bottom) / 2.0, this, m_numObjectsToGrow);
    m_nodes[NE] = QuadTree((m_left + m_right) / 2.0, m_right, m_top, (m_top + m_bottom) / 2.0, this, m_numObjectsToGrow);
    m_nodes[SW] = QuadTree(m_left, (m_left + m_right) / 2.0, (m_top + m_bottom) / 2.0, m_bottom, this, m_numObjectsToGrow);
    m_nodes[SE] = QuadTree((m_left + m_right) / 2.0, m_right, (m_top + m_bottom) / 2.0, m_bottom, this, m_numObjectsToGrow);
}

void QuadTree::moveObjectsToLeaves()
{
    for (int n = 0; n < NodeCount; ++n) {
        for (size_t m = 0; m < m_entities.size(); ++m) {
            if (m_nodes[n].contains(m_entities[m])) {

                m_nodes[n].insert(m_entities[m]);
                m_entities.erase(m_entities.begin() + m);
                --m;
            }
        }
    }
}

//
//void QuadTree::queryRange(std::vector<Entity*>* list, cpBB range) {
//    if (!cpBBIntersects(range, m_boundary)) {
//        Debug::log(Debug::Area::ImportantArea) << "INTERSECTION FAILURE";
//       return; //return unchanged (empty) list.
//    }
//
//    Debug::log(Debug::Area::ImportantArea) << "INTERSECTION SUCCASS";
//
//    for (size_t i = 0; i < m_points.size(); ++i) {
//       const glm::vec2& position = m_points[i]->position();
//        if (cpBBContainsVect(m_boundary, cpv(position.x, position.y))) {
//            list->push_back(m_points[i]);
//        }
//    }
//
//    if (NW == nullptr) {
//        return;
//    }
//
//    NW->queryRange(list, range);
//    NE->queryRange(list, range);
//    SW->queryRange(list, range);
//    SE->queryRange(list, range);
//}
//
