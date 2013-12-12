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
#include "debug.h"

#include <iostream>

#include <QtCore/QStringList>

#include <QtGui/QGuiApplication>
#include <QDebug>

#include <QtQuick/QQuickView>
#include <QOpenGLFunctions>
#include <QOpenGLContext>
#include <QtCore/QCommandLineParser>
#include <QtCore/qlogging.h>

void debugOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            if (msg.startsWith('\n') || msg.startsWith("--")) {
                //don't do func, file, line for separater messages
                fprintf(stderr, "%s \t\t\t\t\t\n\n", localMsg.constData());
            } else {
                fprintf(stderr, "%s \t\t\t\t\t(%s, %s:%u)\n\n", localMsg.constData(), context.function, context.file, context.line);
            }
            break;

        case QtWarningMsg: {
            if (msg.contains("Qt Warning")) {
                //unused, silent qt warnings. but continue to receive our warnings
            } else {
                fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            }
            break;
        }

        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;

        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
    }
}

int main(int argc, char* argv[])
{
//    QOpenGLFunctions funcs(QOpenGLContext::currentContext());
//    bool npot = funcs.hasOpenGLFeature(QOpenGLFunctions::NPOTTextures);
    qInstallMessageHandler(debugOutput);

    const QString version = QString::number(ore_infinium_VERSION_MAJOR) + "." + QString::number(ore_infinium_VERSION_MINOR);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
        "\n"
        "Ore Infinium - An Open Source 2D Block Exploration, Survival, Science Fiction Open World Game\n"
        "\n"
        "Use F11 for toggling performance graphs (client, server frametime, packet count)\n"
        "\n"
        "Authors:\n"
        "       Lead Developer - Shaun Reich <sreich@kde.org>\n"
    );

    //FIXME: unused..we're loud by default. We'll prolly change that later on, but for now i'm fine with it.
    QCommandLineOption startupDebug(QStringList() << "d" << "startupDebug", QCoreApplication::translate("main",
        "Start the game with core debug areas enabled."
        "This applies to initial logging as it starts up. "
        "After it finishes starting, you can always use the ingame UI to enable logging areas (regardless of this switch)."));
    parser.addOption(startupDebug);

    QCommandLineOption testSpatialHash(QStringList() << "test-spatial-hash", QCoreApplication::translate("main",
        "Runs various performance and functionality unit tests on the spatial hash to verify there are no regressions, report and exit."));
    parser.addOption(testSpatialHash);

    // aka noclip
    QCommandLineOption worldViewer(QStringList() << "world-viewer", QCoreApplication::translate("main",
        "Enables special client modes to make the game world easier to troubleshoot (only applicable to client-hosted server mode)."
        "Doesn't attach the player to a physics object and reroutes it"
        "Effectively it is no-clip."));
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
        "--debug-full Enable all terminal output debugging flags"));
    parser.addOption(debugFull);


    //TODO: may wanna run without gui for dedicated server...have the option to, at least

    QCommandLineOption height(QStringList() << "height",
        QCoreApplication::translate("main", "Set height of the window"),
        QCoreApplication::translate("main", "height in pixels"));
    parser.addOption(height);

    QCommandLineOption width(QStringList() << "width",
        QCoreApplication::translate("main", "Set width of the window"),
        QCoreApplication::translate("main", "width in pixels"));
    parser.addOption(width);

    if (1) {
        QGuiApplication app(argc, argv);
        QCoreApplication::setApplicationName("ore-infinium");
        QCoreApplication::setApplicationVersion(version);

        parser.process(app);

        //TODO: option is pointless right now
        if (parser.isSet("startupDebug")) {
            Settings::instance()->setStartupFlag(Settings::StartupFlags::DebugLoggingStartupFlag);
        }

        if (parser.isSet("world-viewer")) {
            Settings::instance()->setStartupFlag(Settings::StartupFlags::WorldViewerStartupFlag);
        }

        if (parser.isSet("no-timeout")) {
            Settings::instance()->setStartupFlag(Settings::StartupFlags::NoTimeoutStartupFlag);
        }

        if (parser.isSet("play-now")) {
            Settings::instance()->setStartupFlag(Settings::StartupFlags::PlayNowStartupFlag);
        }

        if (parser.isSet("no-sky-renderer")) {
            Settings::instance()->setStartupFlag(Settings::StartupFlags::NoSkyRendererStartupFlag);
        }

        if (parser.isSet("test-spatial-hash")) {
            UnitTest* t = new UnitTest();
            t->testSpatialHash();
            delete t;

            exit(0);
        }

        if (parser.isSet("debug-full")) {
            Settings::instance()->setStartupFlag(Settings::StartupFlags::FullDebugStartupFlag);
        }

        if (1) {
            Settings::instance()->debugRendererFlags |=
                Debug::RenderingDebug::ChipmunkShapeRenderingDebug |
                Debug::RenderingDebug::ChipmunkCenterOfMassRenderingDebug |
                Debug::RenderingDebug::ChipmunkJointRenderingDebug |
                Debug::RenderingDebug::ChipmunkAABBRenderingDebug;
        }

        int height;
        int width;
        if (parser.isSet("height") && parser.isSet("width")) {
            int height = parser.value("height").toInt();
            int width = parser.value("width").toInt();

            Settings::instance()->windowHeight = height;
            Settings::instance()->windowWidth = width;
        }

        Game game;
        game.init();

        return app.exec();
    } else {
        //TODO: execDedicatedServer
    }
}
