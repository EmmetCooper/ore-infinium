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
    m_isLeaf(true)
{
}

QuadTree::QuadTree(double left, double right, double top, double bottom, QuadTree* parent, uint32_t _numObjectsToGrow) :
    m_left(left),
    m_right(right),
    m_top(top),
    m_bottom(bottom),
    m_numObjectsToGrow(_numObjectsToGrow),
    m_isLeaf(true),
    m_parent(parent)
{
//    Debug::log(Debug::ImportantArea) << "quadtree ctor, left: " << left << " right: " << right << " top: " << top << " bottom: " << bottom;
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

    assert(false); // should not happen
//    m_entities.push_back(entity);
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
        if (m_nodes[n].containsPoint(x, y, width, height)) {
            childObjects = m_nodes[n].queryRange(x, y, width, height);
            returnedObjects.insert(returnedObjects.end(), childObjects.begin(), childObjects.end());
        }
    }

    return returnedObjects;
}

bool QuadTree::contains(Entity* entity)
{
    const glm::vec2& position = entity->position();
    const glm::vec2& size = entity->sizeMeters();

    return  position.x > m_left &&
            (position.x + size.x) < m_right ;//&&
//            position.y > m_top &&
 //           (position.y + size.y) < m_bottom;
}

bool QuadTree::containsPoint(double x, double y, double width, double height)
{
    return (x >= m_left && x <= m_right) &&
           (y >= m_top && y <= m_bottom);// &&
   //        (width <= m_right && height <= m_bottom);
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
