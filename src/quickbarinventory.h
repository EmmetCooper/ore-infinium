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

#ifndef QUICKINVENTORYMENU_H
#define QUICKINVENTORYMENU_H

#include <vector>
#include <string>

class Item;

class QuickBarInventory
{
public:
    QuickBarInventory();
    ~QuickBarInventory();

    /**
     * Sets the Item stored in this slot
     * @p index 0-based index of the slot and data stored therein
     */
    void setSlot(uint8_t index, Item* item);

    /**
     * Selects/equips the @p index slot
     */
    void selectSlot(uint8_t index) { m_equippedIndex = index; }
    uint8_t equippedIndex() { return m_equippedIndex; }

    uint8_t maxEquippedSlots() { return m_maxEquippedSlots; }

private:
    std::vector<Item*> m_items;

    /// 1-indexed. 1-8
    uint8_t m_maxEquippedSlots = 8;
    uint8_t m_equippedIndex = 0;
};

#endif
