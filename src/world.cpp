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

#include "world.h"
#include "debug.h"

#include "globals.h"

#include "src/server/server.h"
#include "src/server/contactlistener.h"

#include "src/client/client.h"

#include "item.h"
#include "block.h"
#include "torch.h"
#include "game.h"
#include "camera.h"

#include "tilerenderer.h"
#include "lightrenderer.h"
#include "physicsdebugrenderer.h"
#include "particlerenderer.h"
#include "fluidrenderer.h"
#include "skyrenderer.h"

#include "src/time.h"
#include "settings/settings.h"
#include "quickbarinventory.h"
#include "timer.h"
#include "spatialhash.h"
#include "activechunk.h"
#include "tool.h"
#include "vegetation.h"

#include <chipmunk/chipmunk.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <random>
#include <assert.h>
#include <math.h>
#include <fstream>
#include <chrono>
#include <unordered_set>
#include <fstream>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "glm/core/func_common.hpp"

World::World(Entities::Player* mainPlayer, Client* client, Server* server)
    : m_mainPlayer(mainPlayer),
      m_server(server),
      m_client(client)
{
    //FIXME:
//    m_player = new Entities::Player("someframe");
//    m_entities.insert(m_entities.end(), m_player);
    m_blocks.resize(WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    m_uselessEntity = new Entity("player1Standing1", SpriteSheetRenderer::SpriteSheetType::Character);
    m_uselessEntity->setPosition(2300 / PIXELS_PER_METER, 1400 / PIXELS_PER_METER);
    m_entities.insert(m_entities.end(), m_uselessEntity);

    m_treesSpatialHash = new SpatialHash(0.0, 0.0, Block::BLOCK_SIZE * WORLD_COLUMNCOUNT, Block::BLOCK_SIZE * WORLD_ROWCOUNT, 8.0, 4000);
    m_waterSpatialHash = new SpatialHash(0.0, 0.0, Block::BLOCK_SIZE * WORLD_COLUMNCOUNT, Block::BLOCK_SIZE * WORLD_ROWCOUNT, Block::BLOCK_SIZE, SpatialHash::SpatialHashContents::ObjectSpatialHashContents, 4000);

    // time has to be init'ed early on
    m_time = new Time();
    m_time->setTime(13, 59, 0);

    Debug::checkGLError();

    if (m_client) {
        m_camera = new Camera();
        Debug::checkGLError();
        m_spriteSheetRenderer = new SpriteSheetRenderer(m_camera);
        Debug::checkGLError();

        //FIXME:unused m_quadTreeRenderer = new QuadTreeRenderer(m_camera);
        //FIXME: m_quadTreeRenderer->addQuadTree(m_torchesQuadTree);

        Debug::checkGLError();
        m_tileRenderer = new TileRenderer(this, m_camera, m_mainPlayer);
        Debug::checkGLError();

        //that's a HACK
        //Torch* torch = new Torch(glm::vec2(2400 / PIXELS_PER_METER, 1420 / PIXELS_PER_METER));
        Torch* torch = new Torch(m_mainPlayer->position());
        m_torches.push_back(torch);
        m_spriteSheetRenderer->registerSprite(torch);

        Debug::checkGLError();

        m_blockPickingCrosshair = new Sprite("crosshairPickingActive", SpriteSheetRenderer::SpriteSheetType::Entity);
        m_spriteSheetRenderer->registerSprite(m_blockPickingCrosshair);

        Debug::checkGLError();

        m_lightRenderer = new LightRenderer(this, m_camera, m_mainPlayer);
        m_lightRenderer->setTileRendererTexture(m_tileRenderer->fboTexture());

        //FIXME: call each update, and make it only do visible ones
        m_lightRenderer->setTorches(&m_torches);

        m_sky = new SkyRenderer(this, m_camera, m_time);
        m_particleRenderer = new ParticleRenderer(this, m_camera, m_mainPlayer);

        //FIXME: m_fluidRenderer = new FluidRenderer(m_camera, m_mainPlayer);
        //FIXME: m_fluidRenderer->setWaterSpatialHash(m_waterSpatialHash);
    }

    //client doesn't actually load/generate any world
    if (m_server) {
        m_physicsRendererFlushTimer = new Timer();
        float x = Block::BLOCK_SIZE * WORLD_COLUMNCOUNT;
        float y = Block::BLOCK_SIZE * WORLD_ROWCOUNT;

        // cpVect gravity = cpv(0, -100);
        cpVect gravity = cpv(0.0, 9.8);
        m_cpSpace = cpSpaceNew();
        cpSpaceSetGravity(m_cpSpace, gravity);
        cpSpaceSetIterations(m_cpSpace, 10);
        cpSpaceSetSleepTimeThreshold(m_cpSpace, 0.5);
        // cpSpaceAddCollisionHandler(m_cpSpace, 0, 0, &ContactListener::begin, nullptr, nullptr, nullptr, this);

        //FIXME: harcoded and just a value i chose because it seemed to be in this area.
        m_desiredChunks.reserve(8000);

        ////////////////////////

        loadWorld();
        //HACK, as if that wasn't obvious.
        saveWorld();

        Debug::log(Debug::WorldLoaderArea) << "World is x: " << (WORLD_COLUMNCOUNT * Block::BLOCK_SIZE) << " y: " << (WORLD_ROWCOUNT * Block::BLOCK_SIZE) << " meters big";
    }

    //FIXME: saveMap();
}

World::~World()
{
    delete m_tileRenderer;
    delete m_spriteSheetRenderer;
    delete m_lightRenderer;
    delete m_particleRenderer;
    delete m_sky;

    for (auto * torch : m_torches) {
        delete torch;
    }
    m_torches.clear();

    m_blocks.clear();

    for (auto it : m_activeChunks) {
        delete it.second;
    }
    m_activeChunks.clear();

    for (auto * entity : m_entities) {
        delete entity;
    }
    m_entities.clear();

    delete m_blockPickingCrosshair;
    delete m_mainPlayer;
    delete m_treesSpatialHash;
    delete m_waterSpatialHash;
//FIXME:    delete m_torchesQuadTree;

    qDeleteAll(m_players);

    cpSpaceFree(m_cpSpace);

    delete m_camera;
    delete m_time;
    delete m_physicsRendererFlushTimer;
}

void World::addPlayer(Entities::Player* player)
{
    m_players.append(player);

    if (m_server) {
        Debug::log(Debug::Area::ServerEntityCreationArea) << "Adding player to world. Position X :"  << player->position().x << " Y : " << player->position().y;

        const glm::vec2& playerPosition = player->position();

        // destroy an area around the player and where he spawns (blocks) so that he can collide properly with the tiles.

        //FIXME: HACK: this needs improvement. obviously..otherwise it could very easily destroy everything underneath wherever the player left off.
        //clear an area around the player's rect, of tiles, so he can spawn properly.
        const int startX = ((playerPosition.x) / Block::BLOCK_SIZE) - (10);
        const int endX = startX + (20);

        //columns are our X value, rows the Y
        const int startY = ((playerPosition.y) / Block::BLOCK_SIZE) - (10);
        const int endY = startY + (20);
        int index = 0;

        for (int row = startY; row < endY; ++row) {
            for (int column = startX; column < endX; ++column) {
                index = column * WORLD_ROWCOUNT + row;
                assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);
                Block& block = m_blocks[index];
                block.primitiveType = Block::BlockType::NullBlockType;
            }
        }

        //FIXME: doesn't actually work because the server doesn't send clients the list of torches, or when they spawn
        //FIXME: HACK..this is the optimal way, but wtf does push_back give me that bogus error?
//        Torch* torch1 = new Torch(player->position() + glm::vec2(BLOCK_SIZE, BLOCK_SIZE));
//        m_torches.push_back(torch1);
//        m_server->sendItemSpawned(torch1);
//
//        Torch* torch2 = new Torch(player->position() + glm::vec2(BLOCK_SIZE * 4.0, BLOCK_SIZE * 4.0));
//        m_torches.push_back(torch2);
//        m_server->sendItemSpawned(torch2);


        //NOTE: you might be asking, why don't we send a chunk? that's because this happens after this function, which is after the
        // player is created. therefore the next calls will be sending player info to everyone else, and then sending the initial world chunk at this player's position.
        // along with initial data for this one client, like world time and such, to get it up to sync.

    } else if (!m_server) {
        m_spriteSheetRenderer->registerSprite(player);
    }
}

void World::removePlayer(Entities::Player* player)
{
    m_players.removeOne(player);
}

void World::updateTilePhysicsObjects()
{
    for (Entities::Player* player : m_players) {
        // mark which chunks we want to be activated within this players viewport

        glm::ivec2 centerTile = glm::ivec2(int(ceil(player->position().x / BLOCK_SIZE)), int(ceil(player->position().y / BLOCK_SIZE)));

        // half of what is specified, to get both left and right sides of the center (player position).
        glm::ivec2 tilesInViewport = glm::ivec2(int(MAX_VIEWPORT_WIDTH * 0.5f), int(MAX_VIEWPORT_HEIGHT * 0.5f));

        // top-left start
        glm::ivec2 start = glm::ivec2(glm::max(centerTile - tilesInViewport, glm::ivec2(0, 0)));

        // bottom-right end
        glm::ivec2 end = glm::ivec2(glm::min(centerTile + tilesInViewport, glm::ivec2(WORLD_COLUMNCOUNT - ACTIVECHUNK_SIZE, WORLD_ROWCOUNT - ACTIVECHUNK_SIZE)));

        //TODO: switch to uint..obviously cannot be negative, as with all things blocks.
        for (int currentRow = start.y; currentRow < end.y; currentRow += ACTIVECHUNK_SIZE) {
            for (int currentColumn = start.x; currentColumn < end.x; currentColumn += ACTIVECHUNK_SIZE) {

                DesiredChunk desiredChunk(currentRow / ACTIVECHUNK_SIZE, currentColumn / ACTIVECHUNK_SIZE);
                m_desiredChunks.insert(desiredChunk);
            }
        }
    }

//    Debug::log(Debug::StartupArea) << "DESIRED CHUNKS SIZE pre-removal: " << desiredChunks.size();
//   Debug::log(Debug::StartupArea) << "DESIRED CHUNKS SIZE post-removal: " << desiredChunks.size();

    // set all refcounts to 0 so that we start from scratch asking if anyone wants any of the chunks
    for (auto& activeChunk : m_activeChunks) {
        activeChunk.second->refcount = 0;
    }

    // increment the refcount for each desired chunk we need
    for (auto& desired : m_desiredChunks) {

        auto it = m_activeChunks.find(desired);
        if (it == m_activeChunks.end()) {
            // active chunk does not exist, create it!
            ActiveChunk* activeChunk = new ActiveChunk(desired.row, desired.column, &m_blocks, m_cpSpace);
            m_activeChunks[desired] = activeChunk;

            if (m_server->client()) {
                m_server->client()->setActiveChunkCount(m_activeChunks.size());
            }
        } else {
            it->second->refcount += 1;
        }
    }

    // delete all active chunks with a refcount of 0
    for (auto& activeChunk : m_activeChunks) {
        if (activeChunk.second->refcount == 0) {
            delete activeChunk.second;
            m_activeChunks.erase(activeChunk.first);
        }
    }

    m_desiredChunks.clear();
}

Entities::Player* World::playerForID(uint32_t playerID)
{
    for (auto* player : m_players) {
        if (player->playerID() == playerID) {
            return player;
        }
    }

    Debug::assertf(false, "World::findPlayer, player does not exist? that shit's whack");
    return nullptr;
}

void World::render()
{
    assert(m_mainPlayer && !m_server);

    m_lightRenderer->setRenderingEnabled(Settings::instance()->debugRendererFlags & Debug::RenderingDebug::LightRenderingPassDebug);
    assert(m_lightRenderer->lightRenderingEnabled());
    m_tileRenderer->setRenderingEnabled(Settings::instance()->debugRendererFlags & Debug::RenderingDebug::TileRenderingPassDebug);

    //Sky at bottom layer

    //TODO render tilemap..

    if (Settings::instance()->debugRendererFlags & Debug::RenderingDebug::SkyRenderingPassDebug) {
//        m_sky->render();
    }

    //set our view so that the player will stay relative to the view, in the center.
    //HACK    m_window->setView(*m_view);
    m_lightRenderer->renderToFBO();

    m_tileRenderer->render();

    //FIXME: incorporate entities into the pre-lit gamescene FBO, then render lighting as last pass
    m_lightRenderer->renderToBackbuffer();

    //HACK    m_window->setView(m_window->getDefaultView());
    m_spriteSheetRenderer->renderEntities();
    m_spriteSheetRenderer->renderCharacters();

    //FIXME: take lighting into account, needs access to fbos though.
 //   m_fluidRenderer->render();

//    m_particleRenderer->render();

//FIXME unused    m_quadTreeRenderer->render();

//    renderCrosshair();
}

void World::renderCrosshair()
{
    glm::vec2 mouse = m_mainPlayer->mousePositionWorldCoords();

    glm::vec2 crosshairPosition = glm::vec2(Block::BLOCK_SIZE * floor(mouse.x / Block::BLOCK_SIZE), Block::BLOCK_SIZE * floor(mouse.y / Block::BLOCK_SIZE));
    glm::vec2 crosshairOriginOffset = glm::vec2(m_blockPickingCrosshair->sizeMeters().x * 0.5f, m_blockPickingCrosshair->sizeMeters().y * 0.5f);
    glm::vec2 crosshairFinalPosition = glm::vec2(crosshairPosition.x + crosshairOriginOffset.x, crosshairPosition.y + crosshairOriginOffset.y);

    m_blockPickingCrosshair->setPosition(crosshairFinalPosition);
}

void World::update(double elapsedTime)
{
    //only occurs on client side, obviously the server doesn't need to do this stuff
    if (m_client) {
        m_sky->update(elapsedTime);
    }

    if (m_server) {
        for (auto* player : m_players) {
            if (player->mouseLeftButtonHeld()) {
                handlePlayerLeftMouse(player);
            }
        }
    }

    //NOTE: both client and server tick their time. just that the client gets it sync'ed once at connect and further timejumps
    m_time->tick();

    if (m_server) {
        updateTilePhysicsObjects();

        cpSpaceStep(m_cpSpace, FIXED_TIMESTEP);

        if (m_server->client() && m_server->client()->physicsDebugRenderer()) {
            static bool physicsRenderingFlushNeeded = true;

            /// so that we don't flood the renderer with mutex/sync issues..since that's the slowest part
            /// obviously only applicable to physics debug rendering, when client is hosting the server.
            if (m_physicsRendererFlushTimer->milliseconds() >= 500) {
                m_server->client()->physicsDebugRenderer()->iterateShapesInSpace(m_cpSpace);
                m_physicsRendererFlushTimer->reset();
            }
        }
    }

    //NOTE: players are not exactly considered entities. they are, but they aren't
    for (Entity* currentEntity : m_entities) {
        currentEntity->update(elapsedTime, this);
        if (m_server) {
            if (currentEntity->dirtyFlags() & Entity::DirtyFlags::PositionDirty) {
//                m_server->sendPlayerMove(player);
                currentEntity->clearDirtyFlag(Entity::DirtyFlags::PositionDirty);
            }
        }
    }

    for (Entities::Player* player : m_players) {
        player->update(elapsedTime, this);

        assert(player->position().x >= 0.0);
        assert(player->position().y >= 0.0);

        if (m_server) {
            if (player->dirtyFlags() & Entity::DirtyFlags::PositionDirty) {
                m_server->sendPlayerMove(player);
                player->clearDirtyFlag(Entity::DirtyFlags::PositionDirty);

                const glm::ivec2& currentChunkPosition = glm::ivec2(static_cast<int>(player->position().x / Block::BLOCK_SIZE), static_cast<int>(player->position().y / Block::BLOCK_SIZE));

                if (glm::distance(currentChunkPosition, player->lastLoadedChunk) > 20) {
                    Debug::log(Debug::ImportantArea) << " server sending large world chunk..: ";
                    m_server->sendLargeWorldChunkForPlayer(player);
                }
            }
        }
    }

    if (m_mainPlayer) {
        m_camera->centerOn(m_mainPlayer->position());
    }

    //calculateAttackPosition();
}

void World::generateTileMeshes()
{
    /*
    for (int row = 0; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            if (column - 4 >= 0 && row - 4 >= 0) {
                if (column + 4 <= WORLD_COLUMNCOUNT && row + 4 <= WORLD_ROWCOUNT) {
                    m_blocks[column * WORLD_ROWCOUNT + row].meshType = Block::tileMeshingTable.at(calculateTileMeshingType(column, row));
                }
            }
        }
    }
    */
}

uint8_t World::calculateTileMeshingType(int tileX, int tileY) const
{
    const bool left = tileBlendTypeMatch(tileX, tileY, tileX - 1, tileY);
    const bool right = tileBlendTypeMatch(tileX, tileY, tileX + 1, tileY);

    const bool top = tileBlendTypeMatch(tileX, tileY, tileX, tileY - 1);
    const bool bottom = tileBlendTypeMatch(tileX, tileY, tileX, tileY + 1);

    const bool topLeft = tileBlendTypeMatch(tileX, tileY, tileX - 1, tileY - 1);
    const bool bottomLeft = tileBlendTypeMatch(tileX, tileY, tileX - 1, tileY + 1);

    const bool topRight = tileBlendTypeMatch(tileX, tileY, tileX + 1, tileY - 1);
    const bool bottomRight = tileBlendTypeMatch(tileX, tileY, tileX + 1, tileY + 1);

    // behold the motherfucking magic
    const uint8_t result = (1 * topRight) + (2 * bottomRight) + (4 * bottomLeft) + (8 * topLeft) + (16 * top) + (32 * right) + (64 * bottom) + (128 * left);
    return result;
}

bool World::isBlockSolid(const glm::vec2& vecDest) const
{
    const int column = int(std::ceil(vecDest.x) / Block::BLOCK_SIZE);
    const int row = int(std::ceil(vecDest.y) / Block::BLOCK_SIZE);

    int index = column * WORLD_ROWCOUNT + row;
    assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    const uint8_t blockType = World::m_blocks[index].primitiveType;

    //FIXME: do water, lava, doors..what else?
    return  blockType != 0;
}

uint8_t World::blockType(const glm::vec2& vecPoint) const
{
    const int column = int(std::ceil(vecPoint.x) / Block::BLOCK_SIZE);
    const int row = int(std::ceil(vecPoint.y) / Block::BLOCK_SIZE);

    int index = column * WORLD_ROWCOUNT + row;
    assert(index < WORLD_ROWCOUNT * WORLD_COLUMNCOUNT);

    const uint8_t blockType = World::m_blocks[index].primitiveType;

    return  blockType;
}

bool World::tileBlendTypeMatch(int sourceTileX, int sourceTileY, int nearbyTileX, int nearbyTileY) const
{
    bool isMatched = false;
    const int srcIndex = sourceTileX * WORLD_ROWCOUNT + sourceTileY;
    const int nearbyIndex = nearbyTileX * WORLD_ROWCOUNT + nearbyTileY;

    if (m_blocks[srcIndex].primitiveType == m_blocks[nearbyIndex].primitiveType) {
        isMatched = true;
    }

    //TODO future, use blending types..not just tile type is same

    return isMatched;
}

//FIXME: unused..will be used for shooting and such. not for block breaking.
void World::calculateAttackPosition()
{
    /*    const glm::vec2 _viewportCenter = viewportCenter();
     *
     *       const sf::Vector2i mousePos = sf::Mouse::position(*m_window);
     *
     *       glm::vec2 diffVect;
     *       diffVect.x = mousePos.x - _viewportCenter.x;
     *       diffVect.y = mousePos.y - _viewportCenter.y;
     *
     *       const double angle = atan2(diffVect.y, diffVect.x);
     *       const float newX = _viewportCenter.x + cos(angle) * Entities::Player::blockPickingRadius;
     *       const float newY= _viewportCenter.y  + sin(angle) * Entities::Player::blockPickingRadius;
     *       m_relativeVectorToAttack = glm::vec2(newX, newY);
     */
}

void World::loadWorld()
{
    Debug::log(Debug::Area::WorldLoaderArea) << "Loading world!";
    Debug::log(Debug::Area::ImportantArea) << "SIZEOF Block class: " << sizeof(Block);
    Debug::log(Debug::Area::ImportantArea) << "SIZEOF m_blocks: " << (sizeof(Block) * m_blocks.size()) / 1e6 << " MiB";
    generateWorld();
}

void World::generateWorld()
{
    Debug::log(Debug::Area::WorldGeneratorArea) << "Generating a new world.";

    Timer timer;

    std::random_device device;
    std::mt19937 rand(device());
    //FIXME: convert to 1, n
    std::uniform_int_distribution<> distribution(0, 3);

    int lastRow = 0;

    // 200 rows of "sky"
    /*
    for (; lastRow < 15; ++lastRow) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            int index = column * WORLD_ROWCOUNT + lastRow;
            Block& block = m_blocks[index];
            block.primitiveType = Block::BlockType::NullBlockType;
        }
    }
    */
    generateNoise();
//HACK: TAKES TOO LONG    generateOres();

    generateVegetation();

    generateTileMeshes();

    Debug::log(Debug::Area::WorldGeneratorArea) << "Time taken for world generation: " << timer.milliseconds() << " milliseconds";
}

void World::generateNoise()
{
    std::random_device device;
    std::mt19937 rand(device());
    std::uniform_int_distribution<> distribution(0, INT_MAX);

    for (int row = 15; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
//            const int value = add.GetValue(column, row, 2.0) * 0.5 + 1;

            int index = column * WORLD_ROWCOUNT + row;
            Block& block = m_blocks[index];

            if (true) { //HACK:value > 0.1) {
                block.primitiveType = Block::BlockType::DirtBlockType;
                block.wallType = Block::WallType::DirtWallType;
            } else {
                block.wallType = Block::WallType::DirtWallType;
            }
        }
    }
}

//FIXME: unused
void World::generateOres()
{
    assert(0);
    std::random_device device;
    std::mt19937 rand(device());
    std::uniform_int_distribution<> distribution(0, INT_MAX);
    for (int row = WORLD_SEA_LEVEL; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {

            int index = column * WORLD_ROWCOUNT + row;
            Block& block = m_blocks[index];

            block.primitiveType = Block::BlockType::DirtBlockType;
            block.wallType = Block::WallType::DirtWallType;
        }
    }
    /////////////////// done generating dirt world
}

void World::generateVegetation()
{
    for (int column = 0; column < WORLD_COLUMNCOUNT; column += 15) {
        for (int row = 0; row < 20; /* not much further than where the sky stops */ ++row) {

            int index = column * WORLD_ROWCOUNT + row;
            if (m_blocks[index].primitiveType != Block::BlockType::NullBlockType) {
                Vegetation* tree = new Vegetation("tree1", SpriteSheetRenderer::SpriteSheetType::Entity);
                float x = column * Block::BLOCK_SIZE;
                float y = row * Block::BLOCK_SIZE;
                y -= tree->sizeMeters().y;

                tree->setPosition(x, y);
                m_treesSpatialHash->insert(tree);
                break;
            }
        }
    }
}

void World::saveWorld()
{
    /*

     std::cout << "Time taken for map saving: " << elapsedTime << " Milliseconds" << std::endl;
     */

    /*

    std::stringstream ss(std::stringstream::out | std::stringstream::binary);

    int index = 0;
    for (int row = 0; row < WORLD_ROWCOUNT; ++row) {
        for (int column = 0; column < WORLD_COLUMNCOUNT; ++column) {
            index = column * WORLD_ROWCOUNT + row;
            Block* block = &m_blocks[index];
            ss.write((char*)(block), sizeof(Block));
        }
    }


    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    boost::iostreams::zlib_params params;
    params.level = boost::iostreams::zlib::best_compression;

    out.push(boost::iostreams::zlib_compressor(params));
    out.push(ss);

    std::stringstream compressed;
    boost::iostreams::copy(out, compressed);

    //  out.push(file);
    //   char data[5] = {'a', 'b', 'c', 'd', 'e'};
    //    boost::iostreams::copy(boost::iostreams::basic_array_source<char>(data, sizeof(data)), out);

    std::ofstream file("TESTWORLDDATA", std::ios::binary);
    file << compressed.str();
    file.close();

    */

//    file.close();
}
/*
std::string
compress
(
)
{
    std::stringstream compressed;
    std::stringstream decompressed;
    decompressed << data;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(decompressed);
    boost::iostreams::copy(out, compressed);
    return compressed.str();

}
*/

void World::loadChunk(Chunk* chunk)
{
    uint32_t sourceIndex = 0;

    Debug::log(Debug::NetworkClientContinuousArea) << "Loading Chunk START Y: " << chunk->startY() << " ENDY: " << chunk->endY() << " STARTX: " <<  chunk->startX() << " ENDX: " << chunk->endX();

    for (uint32_t row = chunk->startY(); row < chunk->endY(); ++row) {
        for (uint32_t column = chunk->startX(); column < chunk->endX(); ++column) {

            uint32_t index = column * WORLD_ROWCOUNT + row;
            m_blocks.at(index) = chunk->blocks()->at(sourceIndex);
            ++sourceIndex;
        }
    }
}

Chunk World::createChunk(uint32_t startX, uint32_t startY, uint32_t endX, uint32_t endY)
{
    Chunk chunk(startX, startY, endX, endY, &m_blocks);
    return chunk;
}

void World::zoomIn()
{
    m_camera->zoom(m_zoomInFactor);
}

void World::zoomOut()
{
    m_camera->zoom(m_zoomOutFactor);
}

void World::itemQuickBarInventoryDropped(Entities::Player* player, Item* item, uint32_t amount)
{
    Item* droppedItem = item->duplicate();
    assert(droppedItem);

    droppedItem->setStackSize(item->dropStack(amount));

    m_entities.insert(m_entities.end(), droppedItem);
}

void World::itemPrimaryActivated(Entities::Player* player, Item* item)
{
    assert(item);

    if (item->placeable()) {
        // place the item in the world (append to entity list)
        attemptItemPlacement(player);
    } else {
        if (item->type() == Item::ItemType::Tool) {
            Tool* tool = dynamic_cast<Tool*>(item);
            if (tool->toolType() == Tool::ToolType::PickAxe) {
                performBlockAttack(player);
            }
        }

        item->activatePrimary();
    }
}

void World::itemSecondaryActivated(Entities::Player* player, Item* item)
{
    item->activateSecondary();
}

void World::handlePlayerLeftMouse(Entities::Player* player)
{
    // FIXME: HACK: perform the action based on what type of thing is equipped.
    // if it's a sword we attack shit, if it's a pickaxe we attack blocks. for now, lets
    // just try to place the equipped object in the world
    QuickBarInventory* inventory = player->quickBarInventory();
    Item* item = inventory->item(inventory->equippedIndex());

    // null if there's no item equipped (empty slot)
    if (item == nullptr) {
        return;
    }

    itemPrimaryActivated(player, item);;
}

void World::attemptItemPlacement(Entities::Player* player)
{
    QuickBarInventory* inventory = player->quickBarInventory();
    Item* item = inventory->item(inventory->equippedIndex());

    if (item == nullptr) {
        return;
    }

    Debug::fatal(item->stackSize() != 0, Debug::Area::ServerEntityLogicArea, "server: well that's odd, was told that we should place an item, but the item is valid/hanging around, but has no stack size..so it's a count of 0...shouldn't happen.");

    if (player->canPlaceItem() == false) {
        // FIXME: has an arbitrary delay between item placement timings, i'm not sure if this is even needed..maybe.
        // but as of right now it's definitely needed since there's no placement rules, so it will blow through a whole stack in a fraction fo a second.,
        // all placed at the same position.
        return;
    }

    std::vector<Entity*> list;
    float x = player->position().x;//Block::BLOCK_SIZE * WORLD_COLUMNCOUNT;
    float y = player->position().y; //Block::BLOCK_SIZE * WORLD_ROWCOUNT;

    //FIXME:    m_torchesQuadTree->queryRange(list, QuadTree::AABB(QuadTree::XY(x / 2.0, y / 2.0), QuadTree::XY(500, 500)));

    Debug::log(Debug::ImportantArea) << "server torch count: " << m_torches.size();
    // Debug::log(Debug::ImportantArea) << "server torch quadtree query: " << list.size();

    //use player's placement timing and such.
    player->placeItem();

    item->setState(Item::ItemState::Placed);

    glm::vec2 position = glm::vec2(player->mousePositionWorldCoords().x, player->mousePositionWorldCoords().y);
    item->setPosition(position);

    switch (item->type()) {
    case Item::ItemType::Torch: {
        Torch* torch = dynamic_cast<Torch*>(item);

        torch->dropStack(1);

        Torch* newTorch = dynamic_cast<Torch*>(torch->duplicate());
        newTorch->setStackSize(1);
        m_torches.push_back(newTorch);
        //FIXME: m_torchesQuadTree->insert(torch);

        //send the new inventory item count to this player's client.
        m_server->sendQuickBarInventoryItemCountChanged(player, inventory->equippedIndex(), torch->stackSize());
        m_server->sendItemSpawned(newTorch);

        if (torch->stackSize() == 0) {
            //remove it from *our* inventory. the client has already done so.
            player->quickBarInventory()->deleteItem(inventory->equippedIndex());
        }
        break;
    }
    }
}

void World::attemptItemPrimaryAttack(Entities::Player* player)
{

}

void World::spawnItem(Item* item)
{
    switch (item->type()) {
    case Item::ItemType::Torch: {
        Torch* torch = dynamic_cast<Torch*>(item);

        //FIXME: HACK;
        m_torches.push_back(torch);
        break;
    }

    case Item::ItemType::Block: {

        break;
    }
    }

    m_spriteSheetRenderer->registerSprite(item);
}

void World::clientWorldTimeChanged(uint8_t hour, uint8_t minute, uint8_t second)
{
    m_time->setTime(hour, minute, second);
}

void World::tileRemovedPostStepCallback(cpSpace* space, void* obj, void* data)
{
    assert(space);
    World* world = static_cast<World*>(data);

    for (cpShape * shape : world->m_tileShapesToDestroy) {
        ContactListener::BodyUserData* userData = static_cast<ContactListener::BodyUserData*>(cpShapeGetUserData(shape));

        ContactListener::BlockWrapper* blockWrapper = static_cast<ContactListener::BlockWrapper*>(userData->data);

        // find the active chunk associated with this block, we need to remove that otherwise we'll have a double-delete on our hands.
        DesiredChunk desiredChunk(blockWrapper->row / ACTIVECHUNK_SIZE, blockWrapper->column / ACTIVECHUNK_SIZE);
        world->m_activeChunks.at(desiredChunk)->shapeRemoved(shape);

        delete blockWrapper;
        delete userData;

        cpSpaceRemoveShape(space, shape);
        cpShapeDestroy(shape);
    }

    world->m_tileShapesToDestroy.clear();
}

void World::attackTilePhysicsObjectCallback(cpShape* shape, void *data)
{
    ContactListener::BodyUserData* userData = static_cast<ContactListener::BodyUserData*>(cpShapeGetUserData(shape));
    assert(userData);

    if (userData->type != ContactListener::BodyType::BlockBodyType) {
        return;
    }

    ContactListener::BlockWrapper* blockWrapper = static_cast<ContactListener::BlockWrapper*>(userData->data);
    Block* block = blockWrapper->block;

    if (block->primitiveType == Block::BlockType::NullBlockType) {
        return;
    }

    World* world = static_cast<World*>(data);

    //FIXME: decrement health..
    block->primitiveType = Block::BlockType::NullBlockType;

    world->m_tileShapesToDestroy.push_back(shape);
    cpSpaceAddPostStepCallback(world->m_cpSpace, &World::tileRemovedPostStepCallback, nullptr, world);

    uint32_t column = blockWrapper->column;
    uint32_t row = blockWrapper->row;


    Chunk chunk(column, row, column + 1, row + 1, &world->m_blocks);
    world->m_server->sendWorldChunk(&chunk);
}

void World::attackTilePhysicsObject(const glm::vec2& positionToAttack, Entities::Player* player)
{
    //HACK: this presents an issue..we need to notify, upon tile destruction, to the corresponding active chunk, that this tile has been removed so it doesn't attempt to delete that shit.

    cpVect point = cpv(positionToAttack.x, positionToAttack.y);

    cpLayers layers = CP_ALL_LAYERS;
    cpGroup group = CP_NO_GROUP;

    cpSpacePointQuery(m_cpSpace, point, layers, group, &World::attackTilePhysicsObjectCallback, this);
}

void World::performBlockAttack(Entities::Player* player)
{
    glm::vec2 mouse = player->mousePositionWorldCoords();

    const glm::vec2 playerPosition = player->position();

    //FIXME: only doing this for as long as debug mode is here..it works though, trust me. I'm a doctor
#if 0
    // if the attempted block pick location is out of range, do nothing.
    if (mouse.x < playerPosition.x - Entities::Player::blockPickingRadius ||
            mouse.x > playerPosition.x + Entities::Player::blockPickingRadius ||
            mouse.y < playerPosition.y - Entities::Player::blockPickingRadius ||
            mouse.y > playerPosition.y + Entities::Player::blockPickingRadius) {
        return;
    }
#endif

    attackTilePhysicsObject(mouse, player);
}
