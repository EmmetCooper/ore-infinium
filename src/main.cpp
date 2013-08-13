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

#define GLEW_BUILD

//TODO: possible some of these may not be wanted, but what the hell, lets try it!
#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_CXX11
#define GLM_FORCE_INLINE

#include "glm/glm.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "game.h"
#include "config.h"

#include "src/unittest.h"

#include <iostream>

std::vector<std::string> args;

bool contains(const std::string& str)
{
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == str) {
            return true;
        }
    }

    return false;
}

int main(int argc, char* argv[])
{
    bool startupDebugEnabled = false;
    bool worldViewer = false;

    if (argc > 1) {
        //NOTE: we start at 1 because the first element(0) is app name.
        for (int i = 1; i < argc; ++i) {
            args.push_back(std::string(argv[i]));
        }

        if (contains("--help") || contains("-h")) {
            std::cout << "Ore Infinium - An Open Source 2D Block Exploration, Survival, and Open World Game" << '\n' << '\n';

            std::cout << "Options:" << '\n' << '\n';
            std::cout << "-h --help Show this message" << '\n';
            std::cout << "-v --version Show version information" << '\n';
            std::cout << "--authors Show author information" << '\n';
            std::cout << "-d --debug Start the game with core debug areas enabled. This applies to initial logging as it starts up. After it finishes starting, you can use an ingame UI to enable logging areas." << '\n';

            std::cout << '\n' << '\n' << '\n' << "----------------------------------------- debug methods -----------------------------------------" << '\n';
            std::cout << "--test-spatial-hash Runs various unit tests on the spatial hash to verify there are no regressions, report and exit." << '\n';

            exit(0);
        } else if (contains("--authors")) {
            std::cout << "Lead Developer - Shaun Reich <sreich@kde.org>\n";
            exit(0);
        } else if (contains("-d") || contains("--debug")) {
            startupDebugEnabled = true;
        } else if (contains("--test-spatial-hash")) {
            UnitTest* t = new UnitTest();
            t->testSpatialHash();
            delete t;
            exit(0);
        } else if (contains("--world-viewer")) {
            worldViewer = true;
        }
    }

    std::cout << "Ore Infinium Version " << ore_infinium_VERSION_MAJOR << "." << ore_infinium_VERSION_MINOR << "\n";

    Game game;

    if (startupDebugEnabled) {
        game.enableStartupDebugLogging();
    }

    if (worldViewer) {
        game.enableWorldViewer();
    }

    game.init();
}
