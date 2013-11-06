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

// 50px per 1 meter. so that box2d has a range of entity sizes between 0.1 and 10 meters.
static constexpr double PIXELS_PER_METER = 50;
static constexpr float BLOCK_SIZE = 16.0f / PIXELS_PER_METER;

//FIXME: find good values for this. Basically this is as far out as *anyone* can see when they zoom out, regardless of resolution they will all only be able to see
// this many blocks out.
// units are just tile counts, not in meters, just number of tiles.
static constexpr float MAX_VIEWPORT_WIDTH = 200.0f;
static constexpr float MAX_VIEWPORT_HEIGHT = 200.0f;

/// ACTIVECHUNK_SIZE^2 == total tiles within chunk
static constexpr uint32_t ACTIVECHUNK_SIZE = 128;

static constexpr double FIXED_TIMESTEP = 1.0 / 60.0; // hertz

// for chipmunk physics
static constexpr int32_t VELOCITY_ITERATIONS = 6;
static constexpr int32_t POSITION_ITERATIONS = 2;

static constexpr uint32_t WORLD_SEA_LEVEL = 16;

static constexpr uint16_t DEFAULT_PORT = 44543;

#endif
