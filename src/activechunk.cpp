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

#include "activechunk.h"

#include "world.h"

#include <Box2D/Dynamics/b2Body.h>

bool DesiredChunk::operator==(const DesiredChunk& other) const
{
    if (this->row == other.row && this->column == other.column) {
        return true;
    }

    return false;
}

bool DesiredChunk::operator<(const DesiredChunk& rhs) const
{
    if( this->row < rhs.row ) {
        return true;
    }

    if( this->row > rhs.row ) {
        return false;
    }

    if( this->column < rhs.column ) {
        return true;
    }

    if( this->column > rhs.column ) {
        return false;
    }

    return false;
}

ActiveChunk::ActiveChunk(uint32_t row, uint32_t column)
{
    //create all tile physics objects within this area. pos is in chunk indices.
}

ActiveChunk::~ActiveChunk()
{
    for (b2Body* body : m_tileBodies) {
       // delete all tile physics objects within this chunk
    }
}
