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
#include <thread>

UnitTest::UnitTest()
{

}

UnitTest::~UnitTest()
{
    std::cout << '\n' << "unit tests completed, exiting" << '\n';
}

void UnitTest::testSpatialHash()
{
    std::cout << "beginning spatial hash test in 5..4..3...2...1" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "beginning!" << '\n';

    const double width = 85000;
    const double height = 85000;
    SpatialHash* hash = new SpatialHash(0, 0, width, height, 5, 100);

    const int max = 50000;
    for (int i = 0; i < max; ++i) {
        Torch* torch = new Torch(glm::vec2(i, i));
        hash->insert(torch);
    }

    std::cout << "added test items! sleeping for 5 seconds" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Sleep done; querying entire range" << '\n';

    hash->queryRange(0, 0, width, height);

    std::cout << "Range query finished" << '\n';

    std::cout << "spatial hash test finished, sleeping for 5 seconds" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(5));
    delete hash;
}
