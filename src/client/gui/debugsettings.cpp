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

#include "src/client/client.h"
#include <src/debug.h>
#include <src/settings/settings.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>

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

    const bool isChecked = event.GetCurrentElement()->HasAttribute("checked");

    if (id == "ClientRendererArea") {
        Debug::setAreaEnabled(Debug::Area::ClientRendererArea, isChecked);
    } else if (id == "TileRendererArea") {
        Debug::setAreaEnabled(Debug::Area::TileRendererArea, isChecked);
    } else if (id == "SpriteSheetRendererArea") {
        Debug::setAreaEnabled(Debug::Area::SpriteSheetRendererArea, isChecked);
    } else if (id == "LightingRendererArea") {
        Debug::setAreaEnabled(Debug::Area::LightingRendererArea, isChecked);
    } else if (id == "PhysicsArea") {
        Debug::setAreaEnabled(Debug::Area::PhysicsArea, isChecked);
    } else if (id == "AudioArea") {
        Debug::setAreaEnabled(Debug::Area::AudioArea, isChecked);
    } else if (id == "AudioLoaderArea") {
        Debug::setAreaEnabled(Debug::Area::AudioLoaderArea, isChecked);
    } else if (id == "GUILoggerArea") {
        Debug::setAreaEnabled(Debug::Area::GUILoggerArea, isChecked);
    } else if (id == "ShadersArea") {
        Debug::setAreaEnabled(Debug::Area::ShadersArea, isChecked);
    } else if (id == "NetworkClientInitialArea") {
        Debug::setAreaEnabled(Debug::Area::NetworkClientInitialArea, isChecked);
    } else if (id == "NetworkServerInitialArea") {
        Debug::setAreaEnabled(Debug::Area::NetworkServerInitialArea, isChecked);
    } else if (id == "NetworkClientContinuousArea") {
        Debug::setAreaEnabled(Debug::Area::NetworkClientContinuousArea, isChecked);
    } else if (id == "NetworkServerContinuousArea") {
        Debug::setAreaEnabled(Debug::Area::NetworkServerContinuousArea, isChecked);
    } else if (id == "ClientInventoryArea") {
        Debug::setAreaEnabled(Debug::Area::ClientInventoryArea, isChecked);
    } else if (id == "ServerInventoryArea") {
        Debug::setAreaEnabled(Debug::Area::ServerInventoryArea, isChecked);
    } else if (id == "ServerEntityLogicArea") {
        Debug::setAreaEnabled(Debug::Area::ServerEntityLogicArea, isChecked);
    } else if (id == "ImageLoaderArea") {
        Debug::setAreaEnabled(Debug::Area::ImageLoaderArea, isChecked);
    } else if (id == "WorldGeneratorArea") {
        Debug::setAreaEnabled(Debug::Area::WorldGeneratorArea, isChecked);
    } else if (id == "WorldLoaderArea") {
        Debug::setAreaEnabled(Debug::Area::WorldLoaderArea, isChecked);
    } else if (id == "ClientEntityCreationArea") {
        Debug::setAreaEnabled(Debug::Area::ClientEntityCreationArea, isChecked);
    } else if (id == "ServerEntityCreationArea") {
        Debug::setAreaEnabled(Debug::Area::ServerEntityCreationArea, isChecked);
    } else if (id == "SettingsArea") {
        Debug::setAreaEnabled(Debug::Area::SettingsArea, isChecked);
    } else if (id == "StartupArea") {
        Debug::setAreaEnabled(Debug::Area::StartupArea, isChecked);
    }

    //////////////// RENDERER SETTINGS
    if (id == "GUIDebugRendering") {
        if (isChecked) {
            Rocket::Debugger::SetVisible(true);
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::GUIRenderingDebug;
        } else {
            Rocket::Debugger::SetVisible(false);
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::GUIRenderingDebug;
        }
    } else if (id == "LightRenderingPass") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::LightRenderingPassDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::LightRenderingPassDebug;
        }
    } else if (id == "TileRenderingPass") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::TileRenderingPassDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::TileRenderingPassDebug;
        }
    } else if (id == "SkyRenderingPass") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::SkyRenderingPassDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::SkyRenderingPassDebug;
        }
    } else if (id == "ChipmunkShapeRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::ChipmunkShapeRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::ChipmunkShapeRenderingDebug;
        }
    } else if (id == "ChipmunkCenterOfMassRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::ChipmunkCenterOfMassRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::ChipmunkCenterOfMassRenderingDebug;
        }
    } else if (id == "ChipmunkJointRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::ChipmunkJointRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::ChipmunkJointRenderingDebug;
        }
    } else if (id == "ChipmunkAABBRendering") {
        if (isChecked) {
            Settings::instance()->debugRendererFlags |= Debug::RenderingDebug::ChipmunkAABBRenderingDebug;
        } else {
            Settings::instance()->debugRendererFlags &= ~Debug::RenderingDebug::ChipmunkAABBRenderingDebug;
        }
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
//    m_debugSettings = GUI::instance()->context()->LoadDocument("../client/gui/assets/debugSettings.rml");
    m_debugSettings->GetElementById("title")->SetInnerRML("Debug Settings");

    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_debugSettings->GetElementById("tabset"));

    //load initial values
    loadRenderingSettings();

    /////////// LOG DEBUG AREA TAB
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

    //////  RENDERER SETTINGS TAB
    m_debugSettings->GetElementById("GUIDebugRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("LightRenderingPass")->AddEventListener("change", this);
    m_debugSettings->GetElementById("TileRenderingPass")->AddEventListener("change", this);
    m_debugSettings->GetElementById("SkyRenderingPass")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ChipmunkShapeRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ChipmunkCenterOfMassRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ChipmunkJointRendering")->AddEventListener("change", this);
    m_debugSettings->GetElementById("ChipmunkAABBRendering")->AddEventListener("change", this);
}

void DebugSettings::loadRenderingSettings()
{
    int flags = Settings::instance()->debugRendererFlags;

    //all checkboxen are off by default, so just set them to on if they need to be.
    if (flags & Debug::RenderingDebug::GUIRenderingDebug) {
        m_debugSettings->GetElementById("GUIDebugRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::LightRenderingPassDebug) {
        m_debugSettings->GetElementById("LightRenderingPass")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::TileRenderingPassDebug) {
        m_debugSettings->GetElementById("TileRenderingPass")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::SkyRenderingPassDebug) {
        m_debugSettings->GetElementById("SkyRenderingPass")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::ChipmunkShapeRenderingDebug) {
        m_debugSettings->GetElementById("ChipmunkShapeRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::ChipmunkCenterOfMassRenderingDebug) {
        m_debugSettings->GetElementById("ChipmunkCenterOfMassRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::ChipmunkJointRenderingDebug) {
        m_debugSettings->GetElementById("ChipmunkJointRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }

    if (flags & Debug::RenderingDebug::ChipmunkAABBRenderingDebug) {
        m_debugSettings->GetElementById("ChipmunkAABBRendering")->SetAttribute<Rocket::Core::String>("checked", "");
    }
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

