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
#include <QtCore/QCommandLineParser>

int main(int argc, char* argv[])
{
//    QOpenGLFunctions funcs(QOpenGLContext::currentContext());
//    bool npot = funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures);

    const QString version = QString::number(ore_infinium_VERSION_MAJOR) + "." + QString::number(ore_infinium_VERSION_MINOR);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
        "\n"
        "Ore Infinium - An Open Source 2D Block Exploration, Survival, Science Fiction Open World Game\n"
        "\n"
        "F11 for toggling performance graphs (client, server frametime, packet count)\n"
        "\n"
        "Authors:\n"
        "       Lead Developer - Shaun Reich <sreich@kde.org>\n"
    );

//    QCommandLineOption debug(QStringList() << "d" << "debug", QCoreApplication::translate("main",
//        "Start the game with core debug areas enabled."
//        "This applies to initial logging as it starts up. "
//        "After it finishes starting, you can use an ingame UI to enable logging areas."));
//    parser.addOption(showProgressOption);

    QCommandLineOption testSpatialHash(QStringList() << "test-spatial-hash", QCoreApplication::translate("main",
        "Runs various unit tests on the spatial hash to verify there are no regressions, report and exit."));
    parser.addOption(testSpatialHash);

    // aka noclip
    QCommandLineOption worldViewer(QStringList() << "world-viewer", QCoreApplication::translate("main",
        "Enables special client modes to make the game world easier to troubleshoot (only applicable to client-hosted server mode)."));
    parser.addOption(worldViewer);

    QCommandLineOption noTimeout(QStringList() << "no-timeout", QCoreApplication::translate("main",
        "Configures connection timeouts and other things to allow for debugging, especially via e.g. valgrind."));
    parser.addOption(noTimeout);

    QCommandLineOption noSkyRenderer(QStringList() << "no-sky-renderer", QCoreApplication::translate("main",
        "Disables sky renderer (for tricky/slow systems, for debugging. More optimal approaches and settings will exist later)"));
    parser.addOption(noSkyRenderer);

    QCommandLineOption playNow(QStringList() << "play-now", QCoreApplication::translate("main",
        "Hosts and joins a local session immediately (multiplayer) on startup (for fast debugging)."));
    parser.addOption(playNow);

    QCommandLineOption debugFull(QStringList() << "debug-full", QCoreApplication::translate("main",
        "--debug-full Enable all debugging flags (cout)"));
    parser.addOption(debugFull);

   /*

        if (params.contains("--no-sky-renderer")) {
            noSkyRenderer = true;
            params.removeOne("--no-sky-renderer");
        }

        if (params.contains("--debug-full")) {
            fullDebugEnabled = true;
            params.removeOne("--debug-full");
        }

        if (params.contains("--version") || params.contains("-v")) {
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

    qDebug() << "Ore Infinium Version " << version;
    */

    /*
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
    */

    //TODO: may wanna run without gui for dedicated server...have the option to, at least


    if (1) {
        QGuiApplication app(argc, argv);
        QCoreApplication::setApplicationName("ore-infinium");
        QCoreApplication::setApplicationVersion(version);

        parser.process(app);



//        Game game;
//        game.init();
//
//        return app.exec();
    } else {
        //TODO: execDedicatedServer
    }
}
