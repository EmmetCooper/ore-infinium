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

#include "src/client/client.h"
#include "src/client/FboInSGRenderer.h"
#include "src/client/gui/optionsdialogbackend.h"
#include "src/server/server.h"

#include "src/fluids.h"
#include "quickview.h"

#include "settings/settings.h"

//#include <iostream>
//#include <sstream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string>
//#include <fstream>
//
#include <QQmlContext>
#include <QQuickView>
#include <QQmlEngine>

#include <vector>

#include <assert.h>

int Game::m_glFBO;

Game::Game()
{
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

Game::~Game()
{
    delete m_sceneFBOItem;

    m_view->close();
    delete m_view;

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


    ////////////////////
    qmlRegisterType<OptionsDialogBackend>("OptionsDialogBackend", 1, 0, "OptionsDialogBackend");
    qmlRegisterType<FboInSGRenderer>("SceneGraphRendering", 1, 0, "Renderer");

    m_view = new QuickView(m_client);
    m_view->setResizeMode(QQuickView::ResizeMode::SizeViewToRootObject);
    m_view->setMinimumWidth(1600);
    m_view->setMinimumHeight(900);

//    root->setContextProperty("sceneFBOItem", m_sceneFBOItem);

    //m_view->engine()->addImportPath(QString("../client/gui/assets/qml"));

    QQmlEngine* engine = m_view->engine();

    m_client = new Client(m_view);

    m_view->setSource(QUrl("../client/gui/assets/qml/main.qml"));
    m_view->show();

    QQmlContext* root = m_view->engine()->rootContext();
    QQuickItem* rootObject = m_view->rootObject();

    //    QQmlEngine engine;
    //    QQmlComponent component(&engine,
    //                            QUrl::fromLocalFile("MyItem.qml"));
    //    QObject *object = component.create();


    root->setContextProperty("ClientBackend", m_client);

    QObject* obj = rootObject->findChild<QObject*>("renderer");
    assert(obj);

    m_sceneFBOItem = qobject_cast<FboInSGRenderer*>(obj);
    assert(m_sceneFBOItem);


    if (Settings::instance()->startupFlags() & Settings::StartupFlags::WorldViewerStartupFlag) {
        m_client->enableWorldViewing();
    }

    if (Settings::instance()->startupFlags() & Settings::StartupFlags::NoSkyRendererStartupFlag) {
        Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::SkyRenderingPassDebug;
    }

    if (Settings::instance()->startupFlags() & Settings::StartupFlags::FullDebugStartupFlag) {
        Settings::instance()->debugAreas |= Debug::Area::AudioArea | Debug::Area::AudioLoaderArea | Debug::Area::ClientEntityCreationArea |
        Debug::Area::ClientInventoryArea | Debug::Area::ClientRendererArea | Debug::Area::GUILoggerArea | Debug::Area::ImageLoaderArea | Debug::Area::ImportantArea |
        Debug::Area::LightingRendererArea | Debug::Area::NetworkClientContinuousArea | Debug::Area::NetworkClientInitialArea | Debug::Area::NetworkServerContinuousArea |
        Debug::Area::NetworkServerInitialArea | Debug::Area::PhysicsArea | Debug::Area::ServerEntityCreationArea | Debug::Area::ServerEntityLogicArea | Debug::Area::SettingsArea |
        Debug::Area::ShadersArea | Debug::Area::SpriteSheetRendererArea | Debug::Area::StartupArea | Debug::Area::TileRendererArea | Debug::Area::WorldGeneratorArea;
    }

    m_client->init();

    connect(m_sceneFBOItem, SIGNAL(renderCalled()), this, SLOT(render()), Qt::ConnectionType::DirectConnection);

//    connect(node, SIGNAL(renderCalled()), this, SIGNAL(renderCalled()));
//    assert(0);
}

void Game::shutdown()
{
    exit(0);
}

void Game::render()
{
    m_client->paint();
}
