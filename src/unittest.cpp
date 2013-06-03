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

#include "unittest.h"

#include "src/debug.h"

#include "src/spatialhash.h"
#include "torch.h"

UnitTest::UnitTest()
{

}

UnitTest::~UnitTest()
{
    std::cout << '\n' << "unit tests completed, exiting" << '\n';
}

void UnitTest::testSpatialHash()
{
    SpatialHash* hash = new SpatialHash(0, 0, 85000, 85000, 5, 100);

    const int max = 2;//50000;
    for (int i = 0; i < max; ++i) {
        Torch* torch = new Torch(glm::vec2(i, i));
        hash->insert(torch);
    }

    delete hash;
}
