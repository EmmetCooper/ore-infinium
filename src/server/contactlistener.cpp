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
#include <set>
#include <chipmunk/chipmunk.h>

/*
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
*/


int ContactListener::begin(cpArbiter* arbiter, cpSpace* space, void* data)
{

    return 0;
}


void ContactListener::checkBeginContact(ContactListener::BodyUserData* userData)
{
//    Debug::log(Debug::ServerEntityLogicArea) << "BOX2D BEGINN CONTACT";
    switch (userData->type) {
    case BodyType::PlayerFootSensorBodyType: {
        Entities::Player* player = static_cast<Entities::Player*>(userData->data);
        player->addJumpContact();
        break;
    }
    }
}

void ContactListener::checkEndContact(ContactListener::BodyUserData* userData)
{
    switch (userData->type) {
    case BodyType::PlayerFootSensorBodyType:
        Entities::Player* player = static_cast<Entities::Player*>(userData->data);
        player->removeJumpContact();
        break;
    }
}

QueryCallback::QueryCallback(b2World* world)
: m_world(world)
{

}

std::set<b2Fixture*> QueryCallback::fixturesAtPoint(const b2Vec2& point)
{
    std::set<b2Fixture*> fixturesAtPoint;

    for (b2Fixture* fixture : m_fixtures) {
       if (fixture->TestPoint(point)) {
            fixturesAtPoint.insert(fixture);
       }
    }

    m_fixtures.clear();

    return fixturesAtPoint;
}

std::set<b2Fixture*> QueryCallback::fixtures()
{
    std::set<b2Fixture*> fixturesInRange;

    for (b2Fixture* fixture : m_fixtures) {
        fixturesInRange.insert(fixture);
    }

    m_fixtures.clear();

    return fixturesInRange;
}


bool QueryCallback::ReportFixture(b2Fixture* fixture)
{
    Debug::log(Debug::ServerEntityLogicArea) << "FIXTURE REPORTING";

    if (fixture->GetUserData() == nullptr) {
        return true;
    }

   ContactListener::BodyUserData* userData = static_cast<ContactListener::BodyUserData*>(fixture->GetUserData());

   if (userData->type == m_searchType) {
        m_fixtures.push_back(fixture);
    }

    return true;
}
