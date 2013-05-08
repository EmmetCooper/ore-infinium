/******************************************************************************
 *   Copyright (C) 2012 by Shaun Reich <sreich@kde.org>                       *
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

#include "entity.h"

#include "world.h"
#include "debug.h"
#include "block.h"
#include "server/contactlistener.h"

#include <chipmunk.h>

Entity::Entity(const std::string& frameName, SpriteSheetRenderer::SpriteSheetType spriteSheetType)
    : Sprite(frameName, spriteSheetType)
{

}

Entity::Entity(const Entity& entity) : Sprite(entity)
{
    Entity::m_name = entity.name();
    Entity::m_details = entity.details();
    Entity::m_velocity = entity.m_velocity;
}

Entity::~Entity()
{
}

void Entity::setVelocity(const glm::vec2& velocity)
{
    m_velocity = velocity;
}

void Entity::setVelocity(float x, float y)
{
    const glm::vec2 velocity(x, y);
    m_velocity = velocity;
}

void Entity::createPhysicsBody(World* world, const glm::vec2& position)
{
    assert(!m_body);
}

void Entity::update(double elapsedTime, World* world)
{
    //server, physics enabled
    if (m_body) {
        //Debug::log(Debug::Area::ServerEntityLogicArea) << "Sprites present position is: x: " << position().x << " y: " << position().y << " SETTING SPRITE POSITION TO X: " << m_body->GetPosition().x << " Y : " << m_body->GetPosition().y;

        const cpVect& pos = cpBodyGetPos(m_body);
        glm::vec2 position = glm::vec2(pos.x, pos.y);

        this->setPosition(position);

//        glm::vec2 desiredVelocity = m_velocity * glm::vec2(350, 300);
//
//        cpShapeSetSurfaceVelocity(m_mainShape, cpv(desiredVelocity.x, 0.0));
//
//        const cpVect& currentVelocity = cpBodyGetVel(m_body);
//
//        cpFloat velocityChange = desiredVelocity.x - currentVelocity.x;
//
//        cpFloat mass = cpBodyGetMass(m_body);
//        cpVect impulse = cpv(mass * velocityChange, 0.0);
//
//        cpBodyApplyImpulse(m_body, impulse, cpvzero);
//
    }
}

void Entity::setPosition(float x, float y)
{
    m_dirtyFlags |= DirtyFlags::PositionDirty;
    Sprite::setPosition(x, y);
}

void Entity::setPosition(const glm::vec2& vect)
{
    m_dirtyFlags |= DirtyFlags::PositionDirty;
    Sprite::setPosition(vect);
}

uint32_t Entity::dirtyFlags() const
{
    return m_dirtyFlags;
}

void Entity::clearDirtyFlag(uint32_t dirtyFlag)
{
    m_dirtyFlags &= ~dirtyFlag;
}

