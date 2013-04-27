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

#ifndef ACTIVECHUNK_H
#define ACTIVECHUNK_H

#include <map>
#include <string>
#include <vector>
#include "block.h"

class b2World;
class b2Body;

struct DesiredChunk
{
    DesiredChunk(uint32_t _row, uint32_t _column) : row(_row), column(_column)
    {

    }

    bool operator==(const DesiredChunk& other) const;
    bool operator<(const DesiredChunk& rhs) const;

   uint32_t row;
   uint32_t column;
};

class ActiveChunk
{
public:
    /**
     * @p row @p column are in active chunk indexes (tilemap index divided by ACTIVECHUNK_SIZE)
     */
    ActiveChunk(uint32_t row, uint32_t column, std::vector<Block>* blocks, b2World* box2DWorld, b2Body* mainTileBody);
    ~ActiveChunk();


public:
    uint32_t refcount = 1;

private:
    std::vector<b2Body*> m_tileBodies;
    std::vector<Block>* m_blocks = nullptr;
    b2World* m_box2DWorld = nullptr;
    b2Body* m_mainTileBody = nullptr;
};

#endif
