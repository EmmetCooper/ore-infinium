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

#include <chipmunk/chipmunk.h>

class QuadTreeRenderer;
struct cpBB;
struct cpVect;

class QuadTree
{
public:
    enum Node {
        NW = 0,
        NE,
        SW,
        SE,
        NodeCount
    };

public:
    QuadTree();
    QuadTree(double left, double right, double top, double bottom, QuadTree* parent = nullptr, uint32_t numObjectsToGrow = 3);

    ~QuadTree();

    void insert(Entity* entity);

    void clear();

    std::vector<Entity*> queryRange(double x, double y, double width, double height);

private:
    double m_left;
    double m_right;
    double m_top;
    double m_bottom;

    uint32_t m_numObjectsToGrow;

    QuadTree* m_nodes = nullptr;

    bool m_isLeaf;

    bool contains(Entity* entity);
    bool containsPoint(double x, double y, double width, double height);

    void createLeaves();
    void moveObjectsToLeaves();


    QuadTree *m_parent = nullptr;

    std::vector<Entity*> m_entities;

    friend class QuadTreeRenderer;
};



#endif // __QUADTREE__
