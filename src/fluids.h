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

#ifndef FLUIDS_H
#define FLUIDS_H

#include <stdint.h>
#include <vector>

class Fluids
{
public:
    enum FluidType {
        Water = 0,
        Lava = 1
    };

    /// MUST be called otherwise undefined behavior, to initialize the list of fluid types
    static void initFluidTypes();

    struct FluidProperties {
        float maxCompress = 0.0f;
        float maxMass = 0.0f;
        float minMass = 0.0f;

        float bouyancy = 0.0f;
        float damage = 0.0f;
    };

    static std::vector<FluidProperties> fluidTypes;

    Fluids(FluidType type);
    ~Fluids();

private:
    FluidType m_type;
};

#endif
