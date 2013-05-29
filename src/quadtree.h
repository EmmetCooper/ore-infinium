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

class QuadTree {
public:
    QuadTree(QuadTree* parent, cpBB bb);
    QuadTree(QuadTree* parent, cpBB bb, size_t nodeCapacity);

    bool insert(Entity* entity);
    void queryRange(std::vector<Entity*> & emptyInputList, cpBB bb);

    void clear();

private:
    void subdivide();

    cpBB m_boundary;
    size_t m_nodeCapacity;

    // leaves
    QuadTree* NW = nullptr;
    QuadTree* NE = nullptr;
    QuadTree* SW = nullptr;
    QuadTree* SE = nullptr;

    QuadTree *m_parent = nullptr;

    // data
    std::vector<Entity*> m_points;

    friend class QuadTreeRenderer;
};

#endif // __QUADTREE__
