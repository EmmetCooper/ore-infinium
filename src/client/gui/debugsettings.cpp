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

#include "debugsettings.h"
#include "gui.h"

#include "src/client/client.h"
#include <src/debug.h>
#include <src/settings/settings.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

#include <iostream>
#include <sstream>

#include <assert.h>

DebugSettings::DebugSettings(Client* client) : m_client(client)
{
    loadDocument();
}

DebugSettings::~DebugSettings()
{
}

void DebugSettings::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    if (id == "ClientRendererArea") {
        Settings::instance()->debugAreas |= Debug::Area::ClientRendererArea;
    } else if (id == "TileRendererArea") {
        Settings::instance()->debugAreas |= Debug::Area::TileRendererArea;
    } else if (id == "SpriteSheetRendererArea") {
        Settings::instance()->debugAreas |= Debug::Area::SpriteSheetRendererArea;
    } else if (id == "LightingRendererArea") {
        Settings::instance()->debugAreas |= Debug::Area::LightingRendererArea;
    } else if (id == "PhysicsArea") {
        Settings::instance()->debugAreas |= Debug::Area::PhysicsArea;
    } else if (id == "AudioArea") {
        Settings::instance()->debugAreas |= Debug::Area::AudioArea;
    } else if (id == "AudioLoaderArea") {
        Settings::instance()->debugAreas |= Debug::Area::AudioLoaderArea;
    } else if (id == "GUILoggerArea") {
        Settings::instance()->debugAreas |= Debug::Area::GUILoggerArea;
    } else if (id == "ShadersArea") {
        Settings::instance()->debugAreas |= Debug::Area::ShadersArea;
    } else if (id == "NetworkClientInitialArea") {
        Settings::instance()->debugAreas |= Debug::Area::NetworkClientInitialArea;
    } else if (id == "NetworkServerInitialArea") {
        Settings::instance()->debugAreas |= Debug::Area::NetworkServerInitialArea;
    } else if (id == "NetworkClientContinuousArea") {
        Settings::instance()->debugAreas |= Debug::Area::NetworkClientContinuousArea;
    } else if (id == "NetworkServerContinuousArea") {
        Settings::instance()->debugAreas |= Debug::Area::NetworkServerContinuousArea;
    } else if (id == "ClientInventoryArea") {
        Settings::instance()->debugAreas |= Debug::Area::ClientInventoryArea;
    } else if (id == "ServerInventoryArea") {
        Settings::instance()->debugAreas |= Debug::Area::ServerInventoryArea;
    } else if (id == "ServerEntityLogicArea") {
        Settings::instance()->debugAreas |= Debug::Area::ServerEntityLogicArea;
    } else if (id == "ImageLoaderArea") {
        Settings::instance()->debugAreas |= Debug::Area::ImageLoaderArea;
    } else if (id == "WorldGeneratorArea") {
        Settings::instance()->debugAreas |= Debug::Area::WorldGeneratorArea;
    } else if (id == "WorldLoaderArea") {
        Settings::instance()->debugAreas |= Debug::Area::WorldLoaderArea;
    } else if (id == "ClientEntityCreationArea") {
        Settings::instance()->debugAreas |= Debug::Area::ClientEntityCreationArea;
    } else if (id == "ServerEntityCreationArea") {
        Settings::instance()->debugAreas |= Debug::Area::ServerEntityCreationArea;
    } else if (id == "SettingsArea") {
        Settings::instance()->debugAreas |= Debug::Area::SettingsArea;
    } else if (id == "StartupArea") {
        Settings::instance()->debugAreas |= Debug::Area::StartupArea;
    }
}

/*
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
    StartupArea = 1 << 22
    */

void DebugSettings::loadDocument()
{
    m_debugSettings = GUI::instance()->context()->LoadDocument("../client/gui/assets/debugSettings.rml");
    m_debugSettings->GetElementById("title")->SetInnerRML("Debug Settings");

//    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));

    m_debugSettings->GetElementById("ClientRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("TileRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("SpriteSheetRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("LightingRendererArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("PhysicsArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("AudioArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("AudioLoaderArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("GUILoggerArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ShadersArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkClientInitialArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkServerInitialArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkClientContinuousArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("NetworkServerContinuousArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ClientInventoryArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ServerInventoryArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ServerEntityLogicArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ImageLoaderArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("WorldGeneratorArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("WorldLoaderArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ClientEntityCreationArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ServerEntityCreationArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("SettingsArea")->AddEventListener("change", this);
    m_debugSettings->GetElementById("StartupArea")->AddEventListener("change", this);

//    m_debugSettings->GetElementById("sendButton")->AddEventListener("click", this);
//    m_debugSettings->GetElementById("inputLine")->AddEventListener("keydown", this);
}

void DebugSettings::show()
{
    m_debugSettings->Show();
}

void DebugSettings::hide()
{
    m_debugSettings->Hide();
}

bool DebugSettings::visible()
{
    return m_debugSettings->IsVisible();
}

Rocket::Core::ElementDocument* DebugSettings::document()
{
    return m_debugSettings;
}

