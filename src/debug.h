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

#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <sstream>

#include <QDebug>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ORE_CLIENT_RENDERER)
Q_DECLARE_LOGGING_CATEGORY(ORE_TILE_RENDERER)
Q_DECLARE_LOGGING_CATEGORY(ORE_SPRITE_SHEET_RENDERER)
Q_DECLARE_LOGGING_CATEGORY(ORE_LIGHTING_RENDERER)
Q_DECLARE_LOGGING_CATEGORY(ORE_PHYSICS)
Q_DECLARE_LOGGING_CATEGORY(ORE_AUDIO)
Q_DECLARE_LOGGING_CATEGORY(ORE_AUDIO_LOADER)
Q_DECLARE_LOGGING_CATEGORY(ORE_GUI_LOGGER)
Q_DECLARE_LOGGING_CATEGORY(ORE_SHADERS)
Q_DECLARE_LOGGING_CATEGORY(ORE_NETWORK_CLIENT_INITIAL)
Q_DECLARE_LOGGING_CATEGORY(ORE_NETWORK_SERVER_INITIAL)
Q_DECLARE_LOGGING_CATEGORY(ORE_NETWORK_CLIENT_CONTINUOUS)
Q_DECLARE_LOGGING_CATEGORY(ORE_NETWORK_SERVER_CONTINUOUS)
Q_DECLARE_LOGGING_CATEGORY(ORE_CLIENT_INVENTORY)
Q_DECLARE_LOGGING_CATEGORY(ORE_SERVER_INVENTORY)
Q_DECLARE_LOGGING_CATEGORY(ORE_SERVER_ENTITY_LOGIC)
Q_DECLARE_LOGGING_CATEGORY(ORE_IMAGE_LOADER)
Q_DECLARE_LOGGING_CATEGORY(ORE_WORLD_GENERATOR)
Q_DECLARE_LOGGING_CATEGORY(ORE_WORLD_LOADER)
Q_DECLARE_LOGGING_CATEGORY(ORE_CLIENT_ENTITY_CREATION)
Q_DECLARE_LOGGING_CATEGORY(ORE_SERVER_ENTITY_CREATION)
Q_DECLARE_LOGGING_CATEGORY(ORE_SETTINGS)
Q_DECLARE_LOGGING_CATEGORY(ORE_STARTUP)
Q_DECLARE_LOGGING_CATEGORY(ORE_IMPORTANT)


class Debug
{
public:
    enum Area {
        //WARNING: UPDATE THE DEBUG SETTINGS GUI (RML AND CPP) WHEN YOU ADD A FLAG
        ClientRendererArea = 1 << 0,
        TileRendererArea = 1 << 1,
        SpriteSheetRendererArea = 1 << 2,
        LightingRendererArea = 1 << 3,
        PhysicsArea = 1 << 4,
        AudioArea = 1 << 5,
        AudioLoaderArea = 1 << 6,
        GUILoggerArea = 1 << 7,
        ShadersArea = 1 << 8,
        NetworkClientInitialArea = 1 << 9,
        NetworkServerInitialArea = 1 << 10,
        NetworkClientContinuousArea = 1 << 11,
        NetworkServerContinuousArea = 1 << 12,
        ClientInventoryArea = 1 << 13,
        ServerInventoryArea = 1 << 14,
        ServerEntityLogicArea = 1 << 15,
        ImageLoaderArea = 1 << 16,
        WorldGeneratorArea = 1 << 17,
        WorldLoaderArea = 1 << 18,
        ClientEntityCreationArea = 1 << 19,
        ServerEntityCreationArea = 1 << 20,
        SettingsArea = 1 << 21,
        StartupArea = 1 << 22,
        ImportantArea = 1 << 23
    };

    /**
     * The Box2D ones debug renderings are only available for when the client is hosting the server.
     * Otherwise it won't do anything.
     */
    //WARNING: UPDATE THE DEBUG SETTINGS GUI (QML AND CPP) WHEN YOU ADD A FLAG
    enum RenderingDebug {
        GUIRenderingDebug = 1 << 0,
        LightRenderingPassDebug = 1 << 1,
        TileRenderingPassDebug = 1 << 2,
        SkyRenderingPassDebug = 1 << 3,
        ChipmunkShapeRenderingDebug = 1 << 4,
        ChipmunkCenterOfMassRenderingDebug = 1 << 5,
        ChipmunkJointRenderingDebug = 1 << 6,
        ChipmunkAABBRenderingDebug = 1 << 7
    };

    static void setAreaEnabled(Area area, bool enable);

    static void assertf(bool value, const std::string& message);

    static void checkGLError();
    static void checkGLErrorSafe();

    static void registerGLDebugCallback();
};

inline QDebug& operator<<(QDebug debug, const std::string& str)
{
    return debug << QString::fromStdString(str);
}

#endif
