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

#include "contactlistener.h"

#include "src/player.h"

#include <src/debug.h>

ContactListener::ContactListener()
{

}

ContactListener::~ContactListener()
{

}

void ContactListener::BeginContact(b2Contact* contact)
{
    void* fixtureDataA = contact->GetFixtureA()->GetUserData();
    void* fixtureDataB = contact->GetFixtureB()->GetUserData();

    BodyUserData* userDataA = static_cast<BodyUserData*>(fixtureDataA);
    BodyUserData* userDataB = static_cast<BodyUserData*>(fixtureDataB);

    if (userDataA != nullptr) {
        checkBeginContact(userDataA);
    }

    if (userDataB != nullptr) {
        checkBeginContact(userDataB);
    }
}

void ContactListener::EndContact(b2Contact* contact)
{
    void* fixtureDataA = contact->GetFixtureA()->GetUserData();
    void* fixtureDataB = contact->GetFixtureB()->GetUserData();

    BodyUserData* userDataA = static_cast<BodyUserData*>(fixtureDataA);
    BodyUserData* userDataB = static_cast<BodyUserData*>(fixtureDataB);

    if (userDataA != nullptr) {
        checkEndContact(userDataA);
    }

    if (userDataB != nullptr) {
        checkEndContact(userDataB);
    }
}

void ContactListener::checkBeginContact(ContactListener::BodyUserData* userData)
{
    switch (userData->type) {
        case BodyType::PlayerFootSensor: {
            Entities::Player* player = static_cast<Entities::Player*>(userData->data);
            player->addJumpContact();
            break;
        }
    }
}

void ContactListener::checkEndContact(ContactListener::BodyUserData* userData)
{
    switch (userData->type) {
        case BodyType::PlayerFootSensor:
            Entities::Player* player = static_cast<Entities::Player*>(userData->data);
            player->removeJumpContact();
            break;
    }
}

void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    b2ContactListener::PreSolve(contact, oldManifold);
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    b2ContactListener::PostSolve(contact, impulse);
}
