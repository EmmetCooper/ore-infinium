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

//TODO: possible some of these may not be wanted, but what the hell, lets try it!
#define GLM_PRECISION_HIGHP_FLOAT
#define GLM_FORCE_CXX11

#include "glew.h"

#include "glm/glm.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "game.h"
#include "config.h"

#include "src/unittest.h"
#include "settings/settings.h"

#include <iostream>

#include <QtCore/QStringList>

#include <QtGui/QGuiApplication>
#include <QDebug>

#include <QtQuick/QQuickView>
#include <QOpenGLFunctions>
#include <QOpenGLContext>

int main(int argc, char* argv[])
{
//    QOpenGLFunctions funcs(QOpenGLContext::currentContext());
//    bool npot = funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures);

    bool startupDebugEnabled = false;
    bool fullDebugEnabled = false;
    bool worldViewer = false;
    bool noTimeout = false;
    bool playNow = false;
    bool noSkyRenderer = false;

    if (argc > 1) {
        //NOTE: we start at 1 because the first element(0) is app name.
        QStringList params;
        for (int i = 1; i < argc; ++i) {
            QString string(argv[i]);

            if (string.trimmed() != "") {
                params.append(string);
            }

        }

        if (params.contains("--help") || params.contains("-h")) {
            std::cout << "Ore Infinium - An Open Source 2D Block Exploration, Survival, Science Fiction and Open World Game" << '\n' << '\n';

            std::cout << "Options:" << '\n' << '\n';
            std::cout << "-h --help Show this message" << '\n';
            std::cout << "-v --version Show version information" << '\n';
            std::cout << "--authors Show author information" << '\n';
            std::cout << "-d --debug Start the game with core debug areas enabled. This applies to initial logging as it starts up. After it finishes starting, you can use an ingame UI to enable logging areas." << '\n';

            std::cout << '\n' << '\n' << '\n' << "----------------------------------------- debug methods -----------------------------------------" << '\n';
            std::cout << "--test-spatial-hash Runs various unit tests on the spatial hash to verify there are no regressions, report and exit." << '\n';
            std::cout << "--world-viewer Enables special client modes to make the game world easier to troubleshoot (only applicable to client-hosted server mode)." << '\n';
            std::cout << "--no-timeout Configures connection timeouts and other things to allow for debugging, especially via e.g. valgrind." << '\n';
            std::cout << "--no-sky-renderer Disables sky renderer (for tricky/slow systems, for debugging. More optimal approaches and settings will exist later)" << '\n';
            std::cout << "--play-now Hosts and joins a local session immediately on startup (for fast debugging)." << '\n';
            std::cout << "--debug-full Enable all debugging flags (cout)" << '\n';

            exit(0);
        } else if (params.contains("--authors")) {
            std::cout << "Lead Developer - Shaun Reich <sreich@kde.org>\n";
            exit(0);
        }

        if (params.contains("-d") || params.contains("--debug")) {
            startupDebugEnabled = true;
            params.removeOne("-d");
            params.removeOne("--debug");
        }

        if (params.contains("--test-spatial-hash")) {
 //           UnitTest* t = new UnitTest();
  //          t->testSpatialHash();
   //         delete t;
            exit(0);
        }

        if (params.contains("--world-viewer")) {
            worldViewer = true;
            params.removeOne("--world-viewer");
        }

        if (params.contains("--no-timeout")) {
            noTimeout = true;
            params.removeOne("--no-timeout");
        }

        if (params.contains("--play-now")) {
            playNow = true;
            params.removeOne("--play-now");
        }

        if (params.contains("--no-sky-renderer")) {
            noSkyRenderer = true;
            params.removeOne("--no-sky-renderer");
        }

        if (params.contains("--debug-full")) {
            fullDebugEnabled = true;
            params.removeOne("--debug-full");
        }

        if (params.contains("--version" || params.contains("-v")) {
            std::cout << "Ore Infinium Version " << ore_infinium_VERSION_MAJOR << "." << ore_infinium_VERSION_MINOR << "\n";
            exit(0);
        }

        ////// END OF PARAMS, verify no extraneous ones (aka non-existent params)

        if (params.count() > 0) {
            qDebug() << params << '\n';
            std:: cout << "Parameter not recognized! Bailing out!" << '\n';
            exit(1);
        }
    }

    std::cout << "Ore Infinium Version " << ore_infinium_VERSION_MAJOR << "." << ore_infinium_VERSION_MINOR << "\n";

    if (startupDebugEnabled) {
        Settings::instance()->setStartupFlag(Settings::StartupFlags::DebugLoggingStartupFlag);
    }

    if (worldViewer) {
        Settings::instance()->setStartupFlag(Settings::StartupFlags::WorldViewerStartupFlag);
    }

    if (noTimeout) {
        Settings::instance()->setStartupFlag(Settings::StartupFlags::NoTimeoutStartupFlag);
    }

    if (playNow) {
        Settings::instance()->setStartupFlag(Settings::StartupFlags::PlayNowStartupFlag);
    }

    if (noSkyRenderer) {
        Settings::instance()->setStartupFlag(Settings::StartupFlags::NoSkyRendererStartupFlag);
    }

    if (fullDebugEnabled) {
        Settings::instance()->setStartupFlag(Settings::StartupFlags::FullDebugStartupFlag);
    }

    //TODO: may wanna run without gui for dedicated server...have the option to, at least


    if (1) {
        QGuiApplication app(argc, argv);

        Game game;
        game.init();

        return app.exec();
    } else {
        //TODO: execDedicatedServer
    }
}
