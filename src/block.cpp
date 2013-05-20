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

#include "block.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <assert.h>

Block::Block()
{
}

void Block::update()
{

}

std::vector<Block::BlockStruct> Block::blockTypes;
std::vector<Block::WallStruct> Block::wallTypes;

void Block::initBlockTypes() {
    blockTypes.resize(4);
    blockTypes.at(BlockType::NullBlockType) = BlockStruct("../textures/null.png", false);
    blockTypes.at(BlockType::DirtBlockType) = BlockStruct("../textures/dirt.png", true);
    blockTypes.at(BlockType::StoneBlockType) = BlockStruct("../textures/stone.png", true);
    blockTypes.at(BlockType::CopperBlockType) = BlockStruct("../textures/copper.png", true);

    wallTypes.resize(2);
    wallTypes.at(WallType::NullWallType) = WallStruct("../textures/null.png");
    wallTypes.at(WallType::DirtWallType) = WallStruct("../textures/tileWalls/dirtWallType.png");
}

/*
std::vector<Block::WallStruct> Block::wallTypes = {
    { WallType::DirtWallType, WallStruct("../textures/tileWalls/dirtWallType.png") }
};

std::vector<Block::BlockStruct> Block::blockTypes = {
    { BlockType::NullBlockType, BlockStruct("../textures/null.png", false) },
    { BlockType::DirtBlockType, BlockStruct("../textures/dirt.png", true) },
    { BlockType::StoneBlockType, BlockStruct("../textures/stone.png", true) },
    { BlockType::CopperBlockType, BlockStruct("../textures/copper.png", true) }
};
*/


std::map<uint8_t, uint8_t> Block::tileMeshingTable = {
    { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 2 },
    { 4, 3 }, { 5, 4 }, { 6, 2 }, { 7, 2 },
    { 8, 5 }, { 9, 5 }, { 10, 6 }, { 11, 6 },
    { 12, 7 }, { 13, 7 }, { 14, 6 }, { 15, 6 },
    { 16, 8 }, { 17, 9 }, { 18, 10 }, { 19, 10 },
    { 20, 8 }, { 21, 9 }, { 22, 10 }, { 23, 10 },
    { 24, 11 }, { 25, 11 }, { 26, 12 }, { 27, 12 },
    { 28, 11 }, { 29, 11 }, { 30, 12 }, { 31, 12 },
    { 32, 13 }, { 33, 14 }, { 34, 15 }, { 35, 15 },
    { 36, 16 }, { 37, 17 }, { 38, 15 }, { 39, 15 },
    { 40, 5 }, { 41, 5 }, { 42, 6 }, { 43, 6 },
    { 44, 7 }, { 45, 7 }, { 46, 6 }, { 47, 6 },
    { 48, 18 }, { 49, 19 }, { 50, 20 }, { 51, 20 },
    { 52, 18 }, { 53, 19 }, { 54, 20 }, { 55, 20 },
    { 56, 11 }, { 57, 11 }, { 58, 12 }, { 59, 12 },
    { 60, 11 }, { 61, 11 }, { 62, 12 }, { 63, 12 },
    { 64, 21 }, { 65, 22 }, { 66, 23 }, { 67, 23 },
    { 68, 24 }, { 69, 25 }, { 70, 23 }, { 71, 23 },
    { 72, 26 }, { 73, 26 }, { 74, 27 }, { 75, 27 },
    { 76, 28 }, { 77, 28 }, { 78, 27 }, { 79, 27 },
    { 80, 29 }, { 81, 30 }, { 82, 31 }, { 83, 31 },
    { 84, 29 }, { 85, 30 }, { 86, 31 }, { 87, 31 },
    { 88, 32 }, { 89, 32 }, { 90, 33 }, { 91, 33 },
    { 92, 32 }, { 93, 32 }, { 94, 33 }, { 95, 33 },
    { 96, 21 }, { 97, 22 }, { 98, 23 }, { 99, 23 },
    { 100, 24 }, { 101, 25 }, { 102, 23 }, { 103, 23 },
    { 104, 26 }, { 105, 26 }, { 106, 27 }, { 107, 27 },
    { 108, 28 }, { 109, 28 }, { 110, 27 }, { 111, 27 },
    { 112, 29 }, { 113, 30 }, { 114, 31 }, { 115, 31 },
    { 116, 29 }, { 117, 30 }, { 118, 31 }, { 119, 31 },
    { 120, 32 }, { 121, 32 }, { 122, 33 }, { 123, 33 },
    { 124, 32 }, { 125, 32 }, { 126, 33 }, { 127, 33 },
    { 128, 34 }, { 129, 35 }, { 130, 36 }, { 131, 36 },
    { 132, 37 }, { 133, 38 }, { 134, 36 }, { 135, 36 },
    { 136, 39 }, { 137, 39 }, { 138, 40 }, { 139, 40 },
    { 140, 41 }, { 141, 41 }, { 142, 40 }, { 143, 40 },
    { 144, 8 }, { 145, 9 }, { 146, 10 }, { 147, 10 },
    { 148, 8 }, { 149, 9 }, { 150, 10 }, { 151, 10 },
    { 152, 11 }, { 153, 11 }, { 154, 12 }, { 155, 12 },
    { 156, 11 }, { 157, 11 }, { 158, 12 }, { 159, 12 },
    { 160, 42 }, { 161, 43 }, { 162, 44 }, { 163, 44 },
    { 164, 45 }, { 165, 46 }, { 166, 44 }, { 167, 44 },
    { 168, 39 }, { 169, 39 }, { 170, 40 }, { 171, 40 },
    { 172, 41 }, { 173, 41 }, { 174, 40 }, { 175, 40 },
    { 176, 18 }, { 177, 19 }, { 178, 20 }, { 179, 20 },
    { 180, 18 }, { 181, 19 }, { 182, 20 }, { 183, 20 },
    { 184, 11 }, { 185, 11 }, { 186, 12 }, { 187, 12 },
    { 188, 11 }, { 189, 11 }, { 190, 12 }, { 191, 12 },
    { 192, 21 }, { 193, 22 }, { 194, 23 }, { 195, 23 },
    { 196, 24 }, { 197, 25 }, { 198, 23 }, { 199, 23 },
    { 200, 26 }, { 201, 26 }, { 202, 27 }, { 203, 27 },
    { 204, 28 }, { 205, 28 }, { 206, 27 }, { 207, 27 },
    { 208, 29 }, { 209, 30 }, { 210, 31 }, { 211, 31 },
    { 212, 29 }, { 213, 30 }, { 214, 31 }, { 215, 31 },
    { 216, 32 }, { 217, 32 }, { 218, 33 }, { 219, 33 },
    { 220, 32 }, { 221, 32 }, { 222, 33 }, { 223, 33 },
    { 224, 21 }, { 225, 22 }, { 226, 23 }, { 227, 23 },
    { 228, 24 }, { 229, 25 }, { 230, 23 }, { 231, 23 },
    { 232, 26 }, { 233, 26 }, { 234, 27 }, { 235, 27 },
    { 236, 28 }, { 237, 28 }, { 238, 27 }, { 239, 27 },
    { 240, 29 }, { 241, 30 }, { 242, 31 }, { 243, 31 },
    { 244, 29 }, { 245, 30 }, { 246, 31 }, { 247, 31 },
    { 248, 32 }, { 249, 32 }, { 250, 33 }, { 251, 33 },
    { 252, 32 }, { 253, 32 }, { 254, 33 }, { 255, 33 },
};
