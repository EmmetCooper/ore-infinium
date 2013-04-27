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

#include "activechunk.h"

#include "world.h"
#include "server/contactlistener.h"
#include "debug.h"

#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Body.h>

bool DesiredChunk::operator==(const DesiredChunk& other) const
{
    if (this->row == other.row && this->column == other.column) {
        return true;
    }

    return false;
}

bool DesiredChunk::operator<(const DesiredChunk& rhs) const
{
    if( this->row < rhs.row ) {
        return true;
    }

    if( this->row > rhs.row ) {
        return false;
    }

    if( this->column < rhs.column ) {
        return true;
    }

    if( this->column > rhs.column ) {
        return false;
    }

    return false;
}

ActiveChunk::ActiveChunk(uint32_t row, uint32_t column, std::vector<Block>* blocks, b2World* box2DWorld) :
m_blocks(blocks),
m_box2DWorld(box2DWorld)
{
    //create all tile physics objects within this area. pos is in chunk indices.
    int centerTileX = column * ACTIVECHUNK_SIZE;
    int centerTileY = row * ACTIVECHUNK_SIZE;

    //tile indexes
    //FIXME: HACK obviously
    int startRow = centerTileY;
    int endRow = centerTileY + 80;
    int startColumn = centerTileX;
    int endColumn = centerTileX + 80;

    int count = 0;
    int index = 0;
    for (int currentRow = startRow; currentRow < endRow; ++currentRow) {
        for (int currentColumn = startColumn; currentColumn < endColumn; ++currentColumn) {
            count++;

            index = currentColumn * WORLD_ROWCOUNT + currentRow;
            assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
            Block& block = m_blocks->at(index);

            if ( Block::blockTypeMap.at(block.primitiveType).collides == false) {
                //skip over tiles which are not marked as collideable types. obviously, no physics bodies need to be generated for such cases.
                continue;
            }

            b2Body* body = nullptr;

            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position.Set(Block::BLOCK_SIZE * float(currentColumn) + (Block::BLOCK_SIZE * 0.5f), Block::BLOCK_SIZE * float(currentRow) + (Block::BLOCK_SIZE * 0.5f));

            body = m_box2DWorld->CreateBody(&bodyDef);
            m_tileBodies.push_back(body);

            ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
            userData->type = ContactListener::BodyType::Block;
            userData->data = &m_blocks[index];

            body->SetUserData(userData);

            b2PolygonShape box;
            box.SetAsBox(Block::BLOCK_SIZE * 0.5f , Block::BLOCK_SIZE * 0.5f);

            // create main body's fixture
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &box;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 1.0f;

            body->CreateFixture(&fixtureDef);
        }
    }
}

ActiveChunk::~ActiveChunk()
{
    for (b2Body* body : m_tileBodies) {
        // delete all tile physics objects within this chunk
        delete static_cast<ContactListener::BodyUserData*>(body->GetUserData());
        m_box2DWorld->DestroyBody(body);
    }

    m_tileBodies.clear();
}
