/******************************************************************************
 *   Copyright (C) 2012, 2013 by Shaun Reich <sreich@kde.org>                 *
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

#include "player.h"

#include "src/camera.h"
#include "src/quickbarinventory.h"
#include "src/world.h"
#include <src/server/contactlistener.h>

#include "spritesheetmanager.h"
#include "debug.h"
#include "timer.h"
#include <assert.h>

#include <chipmunk.h>

namespace Entities
{

Player::Player(const std::string& frameName)
    : Entity(frameName, SpriteSheetRenderer::SpriteSheetType::Character)
{
    m_placeableDelayTimer = new Timer();
    m_jumpTimer = new Timer();
    m_sizeMeters = glm::vec2(Block::BLOCK_SIZE * 2, Block::BLOCK_SIZE * 3);
}

Player::~Player()
{
    delete m_placeableDelayTimer;
    delete m_jumpTimer;
    delete m_quickBarInventory;
}

void Player::playerUpdateVelocity(cpBody* body, cpVect gravity, cpFloat damping, cpFloat dt)
{

}

void Player::createPhysicsBody(World* world, const glm::vec2& position)
{
    const glm::vec2 size = this->sizeMeters();

    cpSpace* space = world->cpWorldSpace();

    m_body = cpBodyNew(0.5, INFINITY);
//    m_body->velocity_func =
    cpSpaceAddBody(space, m_body);
    cpBodySetPos(m_body, cpv(position.x, position.y));

    cpShape *groundShape = cpCircleShapeNew(m_body , size.x * 1.5, cpvzero);
    cpShapeSetFriction(groundShape, 17.5);

    cpSpaceAddShape(space, groundShape);

    m_groundShape = groundShape;

    //create dynamic body
//    b2BodyDef bodyDef;
//    bodyDef.type = b2_dynamicBody;
//    bodyDef.position.Set(position.x, position.y);
//
//    m_body = world->cpWorldSpace()
//    // sleeping doesn't make sense for a player body.
//    m_body->SetSleepingAllowed(false);
//
//    ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
//    userData->type = ContactListener::BodyType::Player;
//    userData->data = this;
//    m_body->SetUserData(userData);
//
//    b2CircleShape circleShape;
//    circleShape.m_radius = 0.3f;
//
//    // create main body's fixture
//    b2FixtureDef fixtureDef;
//    fixtureDef.shape = &circleShape;
//    fixtureDef.density = 1.0f;
//    fixtureDef.friction = 0.0f;
//
//    m_body->CreateFixture(&fixtureDef);

    //////////// LOWER BODY

 //   b2CircleShape lowerCircleShape;
 //   b2FixtureDef lowerCircleDef;
 //   lowerCircleDef.shape = &lowerCircleShape;
 //   lowerCircleDef.friction = 0.0f;
 //
 //   lowerCircleShape.m_radius = 0.3f;
 //   lowerCircleShape.m_p = b2Vec2(0.0f, 0.1f);
 //
 //   m_body->CreateFixture(&lowerCircleDef);

    ///////// FOOT

//    b2PolygonShape footBox;
//    b2FixtureDef footSensorFixtureDef;
//    footSensorFixtureDef.shape = &footBox;
//    footSensorFixtureDef.isSensor = true;
//    footBox.SetAsBox(0.2, 0.1, b2Vec2(0.0f, 0.4f), 0.0f);
//
//    b2Fixture* footSensorFixture = m_body->CreateFixture(&footSensorFixtureDef);
//
//    ContactListener::BodyUserData* userDataFoot = new ContactListener::BodyUserData();
//    userDataFoot->type = ContactListener::BodyType::PlayerFootSensor;
//    userDataFoot->data = this;
//    footSensorFixture->SetUserData(userDataFoot);
//

    ///////////////////////////////////////////////////////////////////////////////////////////////////////

//    m_body->SetFixedRotation(true);
}

void Player::move(int32_t directionX, int32_t directionY)
{
    Entity::setVelocity(directionX * movementSpeed, directionY * movementSpeed);
}

void Player::jump()
{
    if (m_body) {
        if (m_jumpTimer->milliseconds() >= m_jumpDelay) {
//            if (m_jumpContacts > 0) {
            cpVect currentVelocity = cpBodyGetVel(m_body);

                cpFloat velocityChange = -5.0;

                float impulse = cpBodyGetMass(m_body) * velocityChange;

//                m_body->ApplyLinearImpulse(b2Vec2(0, impulse), m_body->GetWorldCenter());

                m_jumpTimer->reset();
                cpBodyApplyImpulse(m_body, cpv(0, impulse), cpvzero);
 //           }
        }
    }
}

void Player::addJumpContact()
{
    ++m_jumpContacts;
}

void Player::removeJumpContact()
{
    if (m_jumpContacts == 0) {
        return;
    }

    --m_jumpContacts;
}

void Player::setName(const std::string& name)
{
    m_name = name;
}

std::string Player::name() const
{
    return m_name;
}

void Player::setPlayerID(uint32_t id)
{
    m_playerID = id;
}

uint32_t Player::playerID() const
{
    return m_playerID;
}

bool Player::canAttackPrimary()
{

    //FIXME:
    return true;
}

bool Player::canPlaceItem()
{
    if (m_placeableDelayTimer->milliseconds() >= m_placeableDelay) {
        //player has been forced to wait this long before he can place an item again, so now it's okay to do.
        return true;
    }

    return false;
}

void Player::placeItem()
{
    m_placeableDelayTimer->reset();
}

}
