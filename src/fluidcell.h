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

#ifndef FLUIDCELL_H
#define FLUIDCELL_H

#include "object.h"

#include "src/fluids.h"

#include "glm/glm.hpp"

#include <stdint.h>
#include <vector>

class FluidCell : public Object
{
public:
    FluidCell(const glm::ivec2& cell, Fluids::FluidType type);
    ~FluidCell();

    void setCellPosition(const glm::ivec2& pos);

    glm::ivec2 cellPosition() {
        return m_cellPosition;
    }

private:
    glm::ivec2 m_cellPosition = glm::ivec2(0, 0);
    Fluids::FluidType m_type;
};

#endif
