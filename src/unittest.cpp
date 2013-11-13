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

#include "src/spatialhash.h"
#include "src/torch.h"

#include <cstring>

#include <iostream>
#include <thread>
#include <QTime>

UnitTest::UnitTest()
{

}

UnitTest::~UnitTest()
{
    std::cout << '\n' << "unit tests completed, exiting" << '\n';
}

void UnitTest::testSpatialHash()
{
    std::cout << "beginning spatial hash test in 3 seconds" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "beginning!" << '\n';

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QTime time;
    time.start();

    const double width = 85000.0;
    const double height = 85000.0;
    SpatialHash* hash = new SpatialHash(0.0, 0.0, width, height, 50.0, 100.0);

    const int max = 50000;
    for (int i = 0; i < max; ++i) {
        Torch* torch = new Torch(glm::vec2(i, i));
        hash->insert(torch);
    }

    std::cout << "added test items! sleeping for 3 seconds. Time took: " << time.msec() << " ms" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Sleep done; querying entire range" << '\n';

    time.restart();

    QSet<Sprite*> results;
    hash->queryRange(&results, 0.0, 0.0, width, height);

    std::cout << "Range query finished. Full query count, was: " << results.size() << " should be: " << max << " time took: " << time.msec() << " ms" <<  '\n';

    results.clear();
    delete hash;
    hash = nullptr;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::cout << "sleeping for 3 seconds" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Sleep done; adding test items for next range query" << '\n';

    time.restart();

    hash = new SpatialHash(0.0, 0.0, width, height, 5.0, 100.0);

    {
        Torch* torch = new Torch(glm::vec2(5.0, 5.0));
        torch->setSizeMeters(4.0, 4.0);
        hash->insert(torch);
    }

    {
        Torch* torch = new Torch(glm::vec2(6.0, 6.0));
        torch->setSizeMeters(4.0, 4.0);
        hash->insert(torch);
    }

    {
        Torch* torch = new Torch(glm::vec2(10.0, 10.0));
        torch->setSizeMeters(4.0, 4.0);
        hash->insert(torch);
    }

    std::cout << "done; sleeping for 3 seconds. Time took: " << time.msec() << " ms" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Sleep done; querying range, verifying the propery query results due to the size of the objects instead of positions" << '\n';

    time.restart();

    hash->queryRange(&results, 0.0, 0.0, 5.0, 5.0);

    std::cout << "Range query finished. Query count, was: " << results.size() << " should be: " << "1" <<  " took: " << time.msec() << " ms" << '\n';

    results.clear();
    delete hash;
    hash = nullptr;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::cout << "sleeping for 3 seconds" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Sleep done; adding test items for next range query" << '\n';

    time.restart();

    hash = new SpatialHash(0.0, 0.0, width, height, 5.0, 100.0);

    {
        Torch* torch = new Torch(glm::vec2(1000.0, 1000.0));
        torch->setSizeMeters(800.0, 800.0);
        hash->insert(torch);
    }

    {
//        Torch* torch = new Torch(glm::vec2(6.0, 6.0));
//        torch->setSizeMeters(4.0, 4.0);
//        hash->insert(torch);
    }

    std::cout << "done; Took: " << time.msec() << " ms " << "sleeping for 3 seconds" << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Sleep done; querying range, verifying the propery query results due to the size of the objects instead of positions" << '\n';

    time.restart();

    hash->queryRange(&results, 1400.0, 1400.0, 2000.0, 2000.0);

    std::cout << "Range query finished. Took: " << time.msec() << " ms " << "Query count, was: " << results.size() << " should be: " << "1" << '\n';

    std::cout << "spatial hash test finished, sleeping for 5 seconds" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(5));
    delete hash;
    hash = nullptr;
}
