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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>

//TODO: use constexpr when MSVC 2013+n gets it..sigh. where n could very well be 2

//height
//uint16_t
#define WORLD_ROWCOUNT 8400

//width
//uint16_t
#define WORLD_COLUMNCOUNT 2400

// 50px per 1 meter. so that box2d has a range of entity sizes between 0.1 and 10 meters.
#define PIXELS_PER_METER 50.0

// height is the same as width (they're square)
// Block size in meters.
//double
#define BLOCK_SIZE (16.0 / PIXELS_PER_METER)

//Block size in pixels, should NOT be used for ingame calculations but only for texture thangs.
//double
#define BLOCK_SIZE_PIXELS 16.0

// radius indicating how many pixels out the player can pick blocks
#define blockPickingRadius (BLOCK_SIZE * 8.0f)

//FIXME: MOVE MORE UPSTREAM, halfass attempt at making a unit system, instead of making it pixel based for movement.
#define movementUnits (1.0f / 100.0f)
//HACK: was 10, 1.0 now for debugging
#define movementSpeed (1.0f * movementUnits)

#define maximumChatCharCount 256

//FIXME: find good values for this. Basically this is as far out as *anyone* can see when they zoom out, regardless of resolution they will all only be able to see
// this many blocks out.
// units are just tile counts, not in meters, just number of tiles.
//float
#define MAX_VIEWPORT_WIDTH 200.0f
#define MAX_VIEWPORT_HEIGHT 200.0f

/// ACTIVECHUNK_SIZE^2 == total tiles within chunk
//uint32_t
#define ACTIVECHUNK_SIZE 128

//double
// hertz
#define FIXED_TIMESTEP (1.0 / 60.0)

// for chipmunk physics
// int32_t
#define VELOCITY_ITERATIONS = 6
#define POSITION_ITERATIONS = 2

//uint32_T
#define WORLD_SEA_LEVEL 16

#define MAXPLAYERS 8
#define DEFAULT_PORT 44543

#endif
