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

#include "debug.h"
#include "game.h"
#include "settings/settings.h"

#include <assert.h>
#include <iostream>
#include <sstream>

#include <QMap>

#include "glew.h"

#ifdef _WIN32
# include <windows.h>
#else
# define CALLBACK /*nothing*/
#endif

#ifndef NDEBUG
#endif

Q_LOGGING_CATEGORY(ORE_CLIENT_RENDERER,           "oreinfinium.ClientRenderer")
Q_LOGGING_CATEGORY(ORE_TILE_RENDERER,             "oreinfinium.TileRenderer")
Q_LOGGING_CATEGORY(ORE_SPRITE_SHEET_RENDERER,     "oreinfinium.SpriteSheetRenderer")
Q_LOGGING_CATEGORY(ORE_LIGHTING_RENDERER,         "oreinfinium.LightingRenderer")
Q_LOGGING_CATEGORY(ORE_PHYSICS,                   "oreinfinium.Physics")
Q_LOGGING_CATEGORY(ORE_AUDIO,                     "oreinfinium.Audio")
Q_LOGGING_CATEGORY(ORE_AUDIO_LOADER,              "oreinfinium.AudioLoader")
Q_LOGGING_CATEGORY(ORE_GUI_LOGGER,                "oreinfinium.GuiLogger")
Q_LOGGING_CATEGORY(ORE_SHADERS,                   "oreinfinium.Shaders")
Q_LOGGING_CATEGORY(ORE_NETWORK_CLIENT_INITIAL,    "oreinfinium.NetworkClientInitial")
Q_LOGGING_CATEGORY(ORE_NETWORK_SERVER_INITIAL,    "oreinfinium.NetworkServerInitial")
Q_LOGGING_CATEGORY(ORE_NETWORK_CLIENT_CONTINUOUS, "oreinfinium.NetworkClientContinuous")
Q_LOGGING_CATEGORY(ORE_NETWORK_SERVER_CONTINUOUS, "oreinfinium.NetworkServerContinuous")
Q_LOGGING_CATEGORY(ORE_CLIENT_INVENTORY,          "oreinfinium.ClientInventory")
Q_LOGGING_CATEGORY(ORE_SERVER_INVENTORY,          "oreinfinium.ServerInventory")
Q_LOGGING_CATEGORY(ORE_SERVER_ENTITY_LOGIC,       "oreinfinium.ServerEntityLogic")
Q_LOGGING_CATEGORY(ORE_IMAGE_LOADER,              "oreinfinium.ImageLoader")
Q_LOGGING_CATEGORY(ORE_WORLD_GENERATOR,           "oreinfinium.WorldGenerator")
Q_LOGGING_CATEGORY(ORE_WORLD_LOADER,              "oreinfinium.WorldLoader")
Q_LOGGING_CATEGORY(ORE_CLIENT_ENTITY_CREATION,    "oreinfinium.ClientEntityCreation")
Q_LOGGING_CATEGORY(ORE_SERVER_ENTITY_CREATION,    "oreinfinium.ServerEntityCreation")
Q_LOGGING_CATEGORY(ORE_SETTINGS,                  "oreinfinium.Settings")
Q_LOGGING_CATEGORY(ORE_STARTUP,                   "oreinfinium.Startup")
Q_LOGGING_CATEGORY(ORE_IMPORTANT,                 "oreinfinium.Important")

static QMap<Debug::Area, QLoggingCategory*> makeCategoryMap() {
    QMap<Debug::Area, QLoggingCategory*> categoryMap;
#define MAP_AREA(area, category) categoryMap.insert(Debug::area, &category());
    MAP_AREA(ClientRendererArea, ORE_CLIENT_RENDERER)
    MAP_AREA(TileRendererArea, ORE_TILE_RENDERER)
    MAP_AREA(SpriteSheetRendererArea, ORE_SPRITE_SHEET_RENDERER)
    MAP_AREA(LightingRendererArea, ORE_LIGHTING_RENDERER)
    MAP_AREA(PhysicsArea, ORE_PHYSICS)
    MAP_AREA(AudioArea, ORE_AUDIO)
    MAP_AREA(AudioLoaderArea, ORE_AUDIO_LOADER)
    MAP_AREA(GUILoggerArea, ORE_GUI_LOGGER)
    MAP_AREA(ShadersArea, ORE_SHADERS)
    MAP_AREA(NetworkClientInitialArea, ORE_NETWORK_CLIENT_INITIAL)
    MAP_AREA(NetworkServerInitialArea, ORE_NETWORK_SERVER_INITIAL)
    MAP_AREA(NetworkClientContinuousArea, ORE_NETWORK_CLIENT_CONTINUOUS)
    MAP_AREA(NetworkServerContinuousArea, ORE_NETWORK_SERVER_CONTINUOUS)
    MAP_AREA(ClientInventoryArea, ORE_CLIENT_INVENTORY)
    MAP_AREA(ServerInventoryArea, ORE_SERVER_INVENTORY)
    MAP_AREA(ServerEntityLogicArea, ORE_SERVER_ENTITY_LOGIC)
    MAP_AREA(ImageLoaderArea, ORE_IMAGE_LOADER)
    MAP_AREA(WorldGeneratorArea, ORE_WORLD_GENERATOR)
    MAP_AREA(WorldLoaderArea, ORE_WORLD_LOADER)
    MAP_AREA(ClientEntityCreationArea, ORE_CLIENT_ENTITY_CREATION)
    MAP_AREA(ServerEntityCreationArea, ORE_SERVER_ENTITY_CREATION)
    MAP_AREA(SettingsArea, ORE_SETTINGS)
    MAP_AREA(StartupArea, ORE_STARTUP)
    MAP_AREA(ImportantArea, ORE_IMPORTANT)
#undef MAP_AREA
    return categoryMap;
}

static const QMap<Debug::Area, QLoggingCategory*> categoryMap = makeCategoryMap();

void Debug::assertf(bool value, const std::string& message)
{
    if (!value) {
        std::cout << message << "\n";
        assert(0);
    }
}

void Debug::setAreaEnabled(Debug::Area area, bool enable)
{
    Q_ASSERT(categoryMap.contains(area));

    // enable the LogStream-based debug area
    Settings::instance()->setDebugAreaEnabled(area, enable);

    // enable the Qt logging category
    categoryMap.value(area)->setEnabled(QtDebugMsg, enable);
    categoryMap.value(area)->setEnabled(QtWarningMsg, enable);
}

void Debug::checkGLError()
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
	qCDebug(ORE_CLIENT_RENDERER) << "GL error code: " << error << " STRING:" << gluErrorString(error);
        assert(0);
    }
}

void Debug::checkGLErrorSafe()
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
	qCDebug(ORE_CLIENT_RENDERER) << "GL error code: " << error << " STRING:" << gluErrorString(error);
    }
}

#ifdef GLEW_KHR_debug

static void CALLBACK oreGLDebugCallback(
    unsigned int source, unsigned int type,
    unsigned int id, unsigned int severity,
    int length, const char* message, void* userParam
);

void Debug::registerGLDebugCallback()
{
    glDebugMessageCallback(&oreGLDebugCallback, 0);
}

//COLOR HOWTO: http://www.ibm.com/developerworks/linux/library/l-tip-prompt/ , only for linux..obviously
void CALLBACK oreGLDebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
{
    if (!Settings::instance()->isDebugAreaEnabled(Debug::Area::ShadersArea)) {
        return;
    }

    std::string sourceString;

    sourceString.append("\e[32;40m");
    sourceString.append("\nError Source:");

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        sourceString.append("\e[37;40m");
        sourceString.append("[API]");
        sourceString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceString.append("\e[37;40m");
        sourceString.append("[WINDOW SYSTEM]");
        sourceString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceString.append("\e[37;40m");
        sourceString.append("[SHADER COMPILER]");
        sourceString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceString.append("\e[37;40m");
        sourceString.append("[THIRD_PARTY]");
        sourceString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        sourceString.append("\e[37;40m");
        sourceString.append("[APPLICATION]");
        sourceString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SOURCE_OTHER:
        sourceString.append("\e[37;40m");
        sourceString.append("[OTHER]");
        sourceString.append("\e[37;40m\e[0m");
        break;
    }

    sourceString.append("\e[32;40m\e[0m");

    std::string severityString;
    severityString.append("\e[32;40m");
    severityString.append("\nError Source:");

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severityString.append("\e[37;40m");
        severityString.append("[HIGH]");
        severityString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        severityString.append("\e[37;40m");
        severityString.append("[MEDIUM]");
        severityString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SEVERITY_LOW:
        severityString.append("\e[37;40m");
        severityString.append("[LOW]");
        severityString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityString.append("\e[37;40m");
        severityString.append("[NOTIFICATION]");
        severityString.append("\e[37;40m\e[0m");
        break;
    }

    severityString.append("\e[32;40m\e[0m");

    std::string typeString;
    typeString.append("\e[32;40m");
    typeString.append("\nError Source:");

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeString.append("\e[37;40m");
        typeString.append("[ERROR]");
        typeString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeString.append("\e[37;40m");
        typeString.append("[DEPRECATED BEHAVIOR]");
        typeString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeString.append("\e[37;40m");
        typeString.append("[UNDEFINED BEHAVIOR]");
        typeString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        typeString.append("\e[37;40m");
        typeString.append("[PORTABILITY]");
        typeString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        typeString.append("\e[37;40m");
        typeString.append("[PERFORMANCE]");
        typeString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_TYPE_OTHER:
        typeString.append("\e[37;40m");
        typeString.append("[OTHER]");
        typeString.append("\e[37;40m\e[0m");
        break;

    case GL_DEBUG_TYPE_MARKER:
        typeString.append("\e[37;40m");
        typeString.append("[MARKER]");
        typeString.append("\e[37;40m\e[0m");
        break;
    }

    typeString.append("\e[32;40m\e[0m");

    std::string messageString;
    messageString.append("\e[32;40m");
    messageString.append("\nError Message: ");
    messageString.append("\e[32;40m\e[0m");

    messageString.append("\e[30;41m");
    messageString.append(message);
    messageString.append("\e[30;41m\e[0m");

    std::string idString;
    idString.append("\e[32;40m");
    idString.append("\nError ID: ");
    idString.append("\e[32;40m\e[0m");

    qCDebug(ORE_CLIENT_RENDERER) << "\nOpenGL Error Report: " << sourceString << typeString << idString << severityString << messageString << "\n";
}
#endif
