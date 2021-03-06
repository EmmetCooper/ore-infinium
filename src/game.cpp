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
#include "src/client/graph.h"
#include "src/client/FboInSGRenderer.h"
#include "src/client/gui/optionsdialogbackend.h"
#include "src/client/gui/chatmodel.h"
#include "src/server/server.h"

#include "src/fluids.h"
#include "quickview.h"

#include "settings/settings.h"

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
        Debug::setAreaEnabled(Debug::Area::StartupArea, true);
        Debug::setAreaEnabled(Debug::Area::ImageLoaderArea, true);
        Debug::setAreaEnabled(Debug::Area::GUILoggerArea, true);
        Debug::setAreaEnabled(Debug::Area::SettingsArea, true);
        Debug::setAreaEnabled(Debug::Area::ShadersArea, true);
        Debug::setAreaEnabled(Debug::Area::ImportantArea, true);
    }

    Block::initBlockTypes();
    Fluids::initFluidTypes();

    ////////////////////
    qmlRegisterType<OptionsDialogBackend>("OptionsDialogBackend", 1, 0, "OptionsDialogBackend");
    qmlRegisterType<FboInSGRenderer>("SceneGraphRendering", 1, 0, "Renderer");
    qmlRegisterType<Graph>("Graph", 1, 0, "Graph");
    qmlRegisterType<ChatModel>("ChatModel", 1, 0, "ChatModel");

    m_client = new Client();

    m_view = new QuickView(m_client);
    m_view->setResizeMode(QQuickView::ResizeMode::SizeViewToRootObject);
    m_view->setMinimumWidth(Settings::instance()->windowWidth);
    m_view->setMinimumHeight(Settings::instance()->windowHeight);

//    root->setContextProperty("sceneFBOItem", m_sceneFBOItem);

    //m_view->engine()->addImportPath(QString("../client/gui/assets/qml"));

    QQmlEngine* engine = m_view->engine();

    engine->addImportPath("/home/sreich/ore-infinium/install/client/gui/assets");

    QQmlContext* root = m_view->engine()->rootContext();
    root->setContextProperty("ClientBackend", m_client);

    m_view->setSource(QUrl("../client/gui/assets/qml/main.qml"));

    m_view->show();

    int height = Settings::instance()->windowHeight;
    int width = Settings::instance()->windowWidth;

    QQuickItem* rootObject = m_view->rootObject();
    rootObject->setProperty("width", width);
    rootObject->setProperty("height", height);

    //    QQmlEngine engine;
    //    QQmlComponent component(&engine,
    //                            QUrl::fromLocalFile("MyItem.qml"));
    //    QObject *object = component.create();

    QObject* obj = rootObject->findChild<QObject*>("renderer");
    assert(obj);

    ChatModel* model = rootObject->findChild<ChatModel*>("chatModel");
    Q_ASSERT(model);
    m_client->setChatModel(model);

    m_sceneFBOItem = qobject_cast<FboInSGRenderer*>(obj);
    assert(m_sceneFBOItem);

    if (Settings::instance()->startupFlags() & Settings::StartupFlags::WorldViewerStartupFlag) {
        m_client->enableWorldViewing();
    }

    if (Settings::instance()->startupFlags() & Settings::StartupFlags::NoSkyRendererStartupFlag) {
        Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::SkyRenderingPassDebug;
    }

    if (Settings::instance()->startupFlags() & Settings::StartupFlags::FullDebugStartupFlag) {
        Debug::setAreaEnabled(Debug::Area::AudioArea, true);
        Debug::setAreaEnabled(Debug::Area::AudioLoaderArea, true);
        Debug::setAreaEnabled(Debug::Area::ClientEntityCreationArea, true);
        Debug::setAreaEnabled(Debug::Area::ClientInventoryArea, true);
        Debug::setAreaEnabled(Debug::Area::ClientRendererArea, true);
        Debug::setAreaEnabled(Debug::Area::GUILoggerArea, true);
        Debug::setAreaEnabled(Debug::Area::ImageLoaderArea, true);
        Debug::setAreaEnabled(Debug::Area::ImportantArea, true);
        Debug::setAreaEnabled(Debug::Area::LightingRendererArea, true);
        Debug::setAreaEnabled(Debug::Area::NetworkClientContinuousArea, true);
        Debug::setAreaEnabled(Debug::Area::NetworkClientInitialArea, true);
        Debug::setAreaEnabled(Debug::Area::NetworkServerContinuousArea, true);
        Debug::setAreaEnabled(Debug::Area::NetworkServerInitialArea, true);
        Debug::setAreaEnabled(Debug::Area::PhysicsArea, true);
        Debug::setAreaEnabled(Debug::Area::ServerEntityCreationArea, true);
        Debug::setAreaEnabled(Debug::Area::ServerEntityLogicArea, true);
        Debug::setAreaEnabled(Debug::Area::SettingsArea, true);
        Debug::setAreaEnabled(Debug::Area::ShadersArea, true);
        Debug::setAreaEnabled(Debug::Area::SpriteSheetRendererArea, true);
        Debug::setAreaEnabled(Debug::Area::StartupArea, true);
        Debug::setAreaEnabled(Debug::Area::TileRendererArea, true);
        Debug::setAreaEnabled(Debug::Area::WorldGeneratorArea, true);
    }

    m_client->init();

    connect(m_sceneFBOItem, SIGNAL(renderCalled()), this, SLOT(render()), Qt::ConnectionType::DirectConnection);
}

void Game::shutdown()
{
    exit(0);
}

void Game::render()
{
    m_client->paint();
}
