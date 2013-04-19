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

#ifndef TOOL_H
#define TOOL_H

#include "item.h"

#include <vector>
#include <string>

class Timer;
/**
 */
class Tool : public Item
{
public:
    Tool(const glm::vec2& position);
    ~Tool();

    enum ToolType {
       PickAxe,
       Axe,
       Bucket /// ?
    };

    enum Material {
       Wood,
       Stone,
       Steel,
       Diamond
    };

    /**
     */
    virtual void activatePrimary();

    /**
     */
    virtual void activateSecondary();

    virtual Item* duplicate();

    float attackRadius() const {
        return m_attackRadius;
    }

    void setAttackRadius(float radius) {
        m_attackRadius = radius;
    }

    float primaryAttackDelay() const {
        return m_primaryAttackDelay;
    }

    void setPrimaryAttackDelay(uint32_t delay) {
        m_primaryAttackDelay = delay;
    }

private:
    uint32_t m_primaryAttackDelay = 0;
    Timer* m_primaryAttackTimer = nullptr;

    /// meters
    float m_attackRadius = 10.0f;
};

#endif
