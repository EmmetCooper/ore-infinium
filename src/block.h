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

#ifndef BLOCK_H
#define BLOCK_H

#include "globals.h"

#include <vector>
#include <map>

class Block
{
public:
    Block();

    /// must be called or else all shit will break loose. initializes the block and wall types static vectors/lookup table
    static void initBlockTypes();

    /**
     * Function is called ONLY when this block is considered NOT paused, aka alive.
     * So  only when this block is onscreen or close to it will this function be called.
     */
    void update();

    /**
     * Determines the health and texture of the Block.
     * NOTE: MUST be in sync with index of m_blockTextures
     */
    enum BlockType {
        NullBlockType = 0,
        DirtBlockType,
        StoneBlockType,
        CopperBlockType
    };

    /**
     * Only used for constant/fixed/static wall blocks. in other words, if this block was generated by the world somehow,
     * aka this block is "underground" or some other static type, then the user can't modify the wall type (removing, adding or changing).
     * @sa wallType
     *
     */
    enum WallType {
        NullWallType = 0,
        DirtWallType
    };

    enum BlockFlags {
        /// theoretically more things belong in here. except i ran out of ideas :(
        OnFireBlockFlag = 1 << 0
    };

    struct BlockStruct {
        //TODO: add "flammable"
        BlockStruct(const char *_texture, bool _collides) {
            texture = _texture;
            collides = _collides;
        }

        BlockStruct() {
        }
        const char* texture;

        // I thought about using flags..but this seems better, save for the might-be-fucking-huge-constructor
        // this will be useful for TODO: blocks that hurt or help the player's health, etc. (lava), liquids of types, etc.
        bool collides : 1;

        //TODO: animations..array of textures for animation..for destroying and other shit
    };

    struct WallStruct {
        WallStruct(const char *_texture) {
            texture = _texture;
        }

        WallStruct() {
        }
        const char* texture;
    };

    static std::vector<BlockStruct> blockTypes;
    static std::vector<WallStruct> wallTypes;

    /**
     * A lookup table to represent the 0-255 possibilities for a tile being surrounded,
     * as 47 possible different sprites.
     *
     *
     *
     * for more info, see http://forums.tigsource.com/index.php?topic=9859.15
     * http://www.saltgames.com/2010/a-bitwise-method-for-applying-tilemaps/
     * http://www.gamedev.net/page/resources/_/technical/game-programming/tilemap-based-game-techniques-handling-terrai-r934
     * http://www.angryfishstudios.com/2011/04/adventures-in-bitmasking/
     *
     * My amazing illustration:
     * |********|*********|***********
     * |___8____|____16___|____1____*
     * |___128__|__center_|____32___*
     * |___4____|____64___|____2____*
     * |********|*********|***********
     */
    static std::map<uint8_t, uint8_t> tileMeshingTable;

    /**
     * Which mesh sprite to use, aka subsprite.
     * This is utilized to cleanly decide which exact sprite(e.g. full block, corner pieces, etc.) to show for whatever
     * tile (e.g. dirt, grass) this is.
     * 0-15.
     * For example, @sa primitiveType
     * which does not generally depend on the surroundings.
     *
     * meshType however, is determined by calculating the surrounding tiles and if they are of a simlar type or similar
     * blendType, then it will change the overall look of it.
     *
     * Bottom line: use meshType ONLY for rendering, use primitiveType for everything else. meshType is only a displaying
     * niche of a detail, not a gameplay mechanic
     */
    uint8_t meshType = 0;

    /**
     * The type of tile this is, 0-255 is valid and can be compared with the world's definition of tile types
     * (an enum)
     */
    uint8_t primitiveType = 0;

    /**
     * 1:1 correspondence to the primitive type. just that it's rendered in the background with a darker color.
     */
    uint8_t wallType = 0;

    /**
     * if != 0 (WallType::Null), then this is an "underground wall tile" and the user cannot remove/add/change it in any way.
     * @sa WallType
     */
    uint8_t permanentWallType = 0;

    /// NOTE: block ownership is stored in the Player class, which just stores a list of indices of tiles which the player 'owns'.

    uint8_t flags = 0;
};

#endif
