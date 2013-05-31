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

#include <chipmunk/chipmunk.h>

bool DesiredChunk::operator==(const DesiredChunk& other) const
{
    if (this->row == other.row && this->column == other.column) {
        return true;
    }

    return false;
}

bool DesiredChunk::operator<(const DesiredChunk& rhs) const
{
    if (this->row < rhs.row) {
        return true;
    }

    if (this->row > rhs.row) {
        return false;
    }

    if (this->column < rhs.column) {
        return true;
    }

    if (this->column > rhs.column) {
        return false;
    }

    return false;
}

ActiveChunk::ActiveChunk(uint32_t row, uint32_t column, std::vector<Block>* blocks, cpSpace* cpWorldSpace) :
    m_blocks(blocks),
    m_cpSpace(cpWorldSpace)
{
    //create all tile physics objects within this area. pos is in chunk indices.
    int centerTileX = column * ACTIVECHUNK_SIZE;
    int centerTileY = row * ACTIVECHUNK_SIZE;

    //tile indexes
    //FIXME: HACK obviously
    int startRow = centerTileY;
    int endRow = centerTileY + ACTIVECHUNK_SIZE;
    int startColumn = centerTileX;
    int endColumn = centerTileX + ACTIVECHUNK_SIZE;

    int count = 0;
    int index = 0;
    for (int currentRow = startRow; currentRow < endRow; ++currentRow) {
        for (int currentColumn = startColumn; currentColumn < endColumn; ++currentColumn) {
            count++;

            index = currentColumn * WORLD_ROWCOUNT + currentRow;
            assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
            Block& block = m_blocks->at(index);

            if (Block::blockTypes.at(block.primitiveType).collides == false) {
                //skip over tiles which are not marked as collideable types. obviously, no physics bodies need to be generated for such cases.
                continue;
            }

//
//            b2Vec2 pos = b2Vec2(Block::BLOCK_SIZE * float(currentColumn) + (Block::BLOCK_SIZE * 0.5f), Block::BLOCK_SIZE * float(currentRow) + (Block::BLOCK_SIZE * 0.5f));
//
//            ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
//            userData->type = ContactListener::BodyType::Block;
//            userData->data = &m_blocks[index];
//
//            b2PolygonShape box;
//            box.SetAsBox(Block::BLOCK_SIZE * 0.5f , Block::BLOCK_SIZE * 0.5f, pos, 0.0f);
//            box.m_centroid = b2Vec2(Block::BLOCK_SIZE * float(currentColumn) + (Block::BLOCK_SIZE * 0.5f), Block::BLOCK_SIZE * float(currentRow) + (Block::BLOCK_SIZE * 0.5f));
//
//            // create main body's fixture
//            b2FixtureDef fixtureDef;
//            fixtureDef.shape = &box;
//            fixtureDef.density = 1.0f;
//            fixtureDef.friction = 1.0f;
//            fixtureDef.userData = userData;
//
//            b2Fixture* fixture = m_mainTileBody->CreateFixture(&fixtureDef);
//            m_tileFixtures.push_back(fixture);

            float x = Block::BLOCK_SIZE * float(currentColumn);
            float y = Block::BLOCK_SIZE * float(currentRow);

            cpBB bb = cpBBNew(x, y, x + Block::BLOCK_SIZE, y + BLOCK_SIZE);
            cpShape *tileShape = cpBoxShapeNew2(m_cpSpace->staticBody , bb);
            cpShapeSetFriction(tileShape, 2.7);

            ContactListener::BlockWrapper* blockWrapper = new ContactListener::BlockWrapper();
            blockWrapper->block = &m_blocks->at(index);
            blockWrapper->row = currentRow;
            blockWrapper->column = currentColumn;

            ContactListener::BodyUserData* userData = new ContactListener::BodyUserData();
            userData->type = ContactListener::BodyType::BlockBodyType;
            userData->data = blockWrapper;

            cpShapeSetUserData(tileShape, userData);

            cpSpaceAddShape(m_cpSpace, tileShape);

            m_tileShapes.push_back(tileShape);
        }
    }
    //Debug::log(Debug::StartupArea) << "ACTIVE CHUNK CTOR, tile object count: " << m_tileFixtures.size();
}

ActiveChunk::~ActiveChunk()
{
    for (cpShape * shape : m_tileShapes) {
        ContactListener::BodyUserData* userData = static_cast<ContactListener::BodyUserData*>(cpShapeGetUserData(shape));
        ContactListener::BlockWrapper* blockWrapper = static_cast<ContactListener::BlockWrapper*>(userData->data);

        delete blockWrapper;
        delete userData;

        cpSpaceRemoveShape(m_cpSpace, shape);
        cpShapeFree(shape);
    }

    m_tileShapes.clear();
    //Debug::log(Debug::StartupArea) << "ACTIVE CHUNK DTOR, tile object count: " << m_tileFixtures.size();
}

void ActiveChunk::shapeRemoved(cpShape* shape)
{
    for (auto it = m_tileShapes.begin(); it < m_tileShapes.end(); ++it) {
        if (*it == shape) {
            m_tileShapes.erase(it);
        }
    }
}
