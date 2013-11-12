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

#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include <src/block.h>

struct cpArbiter;
struct cpSpace;

class ContactListener
{
public:
    static int /* cpBool */ begin(cpArbiter* arbiter, cpSpace* space, void* data /* World pointer */);

    enum BodyType {
        InvalidBodyType = -1,
        PlayerBodyType = 0,
        PlayerFootSensorBodyType = 1,
        BlockBodyType
    };

    struct BodyUserData {
        int type = -1;
        void* data = nullptr;
    };

    struct BlockWrapper {
        Block* block;
        uint32_t row;
        uint32_t column;
    };

private:
    static void checkBeginContact(BodyUserData* userData);
    static void checkEndContact(BodyUserData* userData);
};

//FIXME: dead code from b2 port
//
//class QueryCallback : public b2QueryCallback
//{
//public:
//    QueryCallback(b2World* world);
//
//    virtual bool ReportFixture(b2Fixture* fixture);
//
//    std::set<b2Fixture*> fixturesAtPoint(const b2Vec2& point);
//
//    std::set<b2Fixture*> fixtures();
//
//    /**
//     * Call this before calling QueryAABB, it will save all fixtures that meet the criteria
//     * so you can call fixtures() later on and retrieve that list.
//     */
//    void setFixtureSearchType(ContactListener::BodyType type) {
//        m_searchType = type;
//    }
//
//private:
//    b2World* m_world = nullptr;
//
//    ContactListener::BodyType m_searchType;
//
//    std::vector<b2Fixture*> m_fixtures;
//};

#endif
