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

#include "tool.h"

#include "src/timer.h"

#include "debug.h"

Tool::Tool(const glm::vec2& position): Item("torch1Ground1")
{
    Item::setPosition(position);
    Item::setName("Tool-licious");
    Item::setDetails("a motherfucking tool");
    Item::m_maximumStackSize = 1;
    Item::m_stackSize = 1;
    Item::m_type = Item::ItemType::Tool;
    Item::m_properties = Item::ItemProperties::Consumable;
}

Tool::~Tool()
{
}

Item* Tool::duplicate()
{
    Tool* tool = new Tool(Item::position());

    return tool;
}

void Tool::setToolType(uint32_t toolType)
{
    m_toolType = toolType;
    computeFrame();
}

void Tool::setToolMaterial(uint32_t toolMaterial)
{
    m_toolMaterial = toolMaterial;
    computeFrame();
}

void Tool::computeFrame()
{
    if (m_toolType == ToolType::PickAxe) {
        if (m_toolMaterial == ToolMaterial::Wood) {
            setFrameName("pickaxeWooden1");
        }
    }
}

void Tool::activatePrimary()
{
    //TODO: decrement duraability
}

void Tool::activateSecondary()
{

}
