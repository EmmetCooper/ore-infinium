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

#include "game.h"

#include "debug.h"

#include <google/protobuf/stubs/common.h>

#include "client/client.h"
#include "server/server.h"

#include "src/fluids.h"

#include "settings/settings.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <vector>

#include <assert.h>

Game::Game()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

Game::~Game()
{
//    delete m_world;
    enet_deinitialize();
    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();
}

void Game::abort_game(const char* message)
{
    printf("%s \n", message);
    shutdown();
    exit(1);
}

void Game::init()
{
    if (Settings::instance()->startupFlags() & Settings::StartupFlags::DebugLoggingStartupFlag) {
        Settings::instance()->debugAreas |= Debug::Area::StartupArea | Debug::Area::ImageLoaderArea | Debug::Area::GUILoggerArea | Debug::Area::SettingsArea
        | Debug::Area::ShadersArea | Debug::Area::ImportantArea;
    }

    Block::initBlockTypes();
    Fluids::initFluidTypes();

    m_client = new Client();

    if (Settings::instance()->startupFlags() & Settings::StartupFlags::WorldViewerStartupFlag) {
        m_client->enableWorldViewing();
    }

    m_client->init();
}

void Game::execClient()
{
    m_client->exec();
}

void Game::shutdown()
{
    exit(0);
}
