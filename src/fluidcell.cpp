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

#include "fluidcell.h"
#include "block.h"

std::vector<FluidCell::FluidProperties> FluidCell::fluidTypes;

FluidCell::FluidCell(const glm::ivec2& cell, FluidCell::FluidType type)
: m_cellPosition(cell),
  m_type(type)

{

}

FluidCell::~FluidCell()
{

}

void FluidCell::setCellPosition(const glm::ivec2& pos)
{
    m_cellPosition = pos;
    setPosition(glm::vec2(pos.x * Block::BLOCK_SIZE, pos.y * Block::BLOCK_SIZE));
}

void FluidCell::initFluidTypes()
{
    fluidTypes.resize(2);
    FluidProperties waterProperty;
    fluidTypes.at(FluidType::Water) = waterProperty;

    FluidProperties lavaProperty;
    fluidTypes.at(FluidType::Lava) = lavaProperty;
}
