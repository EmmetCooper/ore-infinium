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

#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "block.h"

#include <SDL2/SDL_events.h>
#include <chipmunk/chipmunk.h>

struct cpVect;
struct cpArbiter;

class World;
class Server;
class Timer;
class QuickBarInventory;
class Camera;
class World;

namespace Entities
{
class Player : public Entity
{
public:
    /**
     * Create a player with the starting frame texture
     */
    Player(const std::string& frameName);
    ~Player();

    void enableNoClip() {
        m_noClipEnabled = true;
    }

//    void handleEvent(const SDL_Event& event);

    void setName(const std::string& name);
    std::string name() const;

    void setPlayerID(uint32_t id);
    uint32_t playerID() const;

    virtual void createPhysicsBody(World* world, const glm::vec2& position);

    virtual void update(double elapsedTime, World* world);

    /**
     * Server side only, to asociate and keep track of each player's (clients) mouse position
     */
    void setMousePositionWorldCoords(float x, float y) {
        m_mousePositionWorldCoords = glm::vec2(x, y);
    }
    glm::vec2 mousePositionWorldCoords() const {
        return m_mousePositionWorldCoords;
    }

    void setMouseLeftButtonHeld(bool held) {
        m_mouseLeftButtonHeld = held;
    }
    void setMouseRightButtonHeld(bool held) {
        m_mouseRightButtonHeld = held;
    }
    bool mouseLeftButtonHeld() const {
        return m_mouseLeftButtonHeld;
    }
    bool mouseRightButtonHeld() const {
        return m_mouseRightButtonHeld;
    }

    void setQuickBarInventory(QuickBarInventory* inventory) {
        m_quickBarInventory = inventory;
    }
    QuickBarInventory* quickBarInventory() {
        return m_quickBarInventory;
    }

    bool canAttackPrimary();

    void placeItem();
    bool canPlaceItem();

    /**
     * Accepts player input result, which is a vector from -1 to 1 indicating
     * direction, not magnitude.
     *
     * Only used by server.
     */
    void move(int32_t directionX, int32_t directionY);

    void addJumpContact();
    void removeJumpContact();
    void jump();

    /**
     * Returns the players depth in the world (how many units below sea level. Negative if above.
     */
    int32_t depthBelowSeaLevel();

    //FIXME: ...store a list of indices of blocks that are owned and look those up in a map by the index we want to see ifi t's owned.
    //e.g. pick a block, ask "do we own this index we're trying to pick", yes? -> destroy that and remove player "ownage".
    //    uint16_t playerOwned = 1;

public:

private:
    static void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);
    static void checkEachArbiter(cpBody* body, cpArbiter* arbiter, void* data);

    uint32_t m_maxHealth = 2500;
    uint32_t m_health = m_maxHealth;

    /**
     * Unique and utilized only by players, is not global or related to generic entity id's
     * Is used to identify the players, for knowing which one the network is talking about,
     * and is also very useful for kicking/banning.
     */
    uint32_t m_playerID;

    /// milliseconds
    const uint64_t m_placeableDelay = 500;

    /**
     * NOTE: presently not designed to allow name changing mid-connection. You need to reconnect to do that.
     * (allowing name changes seems likemore of a problem than a solution to anything.)
     */
    std::string m_name;

    /// only utilized by server. as of present, the client stores this in itself
    QuickBarInventory* m_quickBarInventory = nullptr;

    Timer* m_placeableDelayTimer = nullptr;

    /**
     * The mouse position, converted into world coordinates (by the client, and received by server).
     */
    glm::vec2 m_mousePositionWorldCoords = glm::vec2(0.0f, 0.0f);
    bool m_mouseLeftButtonHeld = false;
    bool m_mouseRightButtonHeld = false;

    /**
     * Used by the server, if the current position of the player varies enough against this to need
     * a new chunk to be sent, the server will send it out.
     * NOTE: this refers to the center of the viewport and of the loaded area
     */
    glm::ivec2 lastLoadedChunk = glm::ivec2(0, 0);

    // if > 0 means something is touching our feet, so we can jump.
    uint32_t m_jumpContacts = 0;

    bool m_feetOnGround = false;

    uint16_t m_ping = 0;

    Timer* m_jumpTimer = nullptr;
    uint64_t m_jumpDelay = 300;

    cpShape* m_footShape = nullptr;

    glm::vec2 m_desiredVelocity = glm::vec2(0.0f, 0.0f);

    bool m_noClipEnabled = false;

    friend Server;
    friend World;
};

}

#endif
