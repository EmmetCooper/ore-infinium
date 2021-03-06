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

#include "settings.h"
#include <src/debug.h>

Settings* Settings::s_instance(0);

Settings* Settings::instance()
{
    if (!s_instance)
        s_instance = new Settings();

    return s_instance;
}

Settings::Settings()
    : debugAreas(Debug::Area::ImportantArea | Debug::Area::ShadersArea | Debug::Area::StartupArea),

      m_startupFlags(0)
{
    //turn some useful shit on by default. and some important shit.
    debugRendererFlags |=
        Debug::RenderingDebug::TileRenderingPassDebug |
        Debug::RenderingDebug::LightRenderingPassDebug |
        Debug::RenderingDebug::SkyRenderingPassDebug;
}

Settings::~Settings()
{

}
