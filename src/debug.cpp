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

#include "glew.h"

#ifndef NDEBUG
#endif

LogStream Debug::log(Debug::Area area)
{
    return LogStream(area);
}

void Debug::assertf(bool value, const std::string& message)
{
    if (!value) {
        std::cout << message << "\n";
        assert(0);
    }
}

void Debug::setAreaEnabled(Debug::Area area, bool enable)
{
    Settings::instance()->setDebugAreaEnabled(area, enable);
}

void Debug::fatal(bool value, Debug::Area area, const std::string& message)
{
    if (!value) {
        //HACK so that it actually outputs something regardless.
        Debug::log(Debug::Area::ImportantArea) << "FATAL: " << message;
        assert(0);
    }
}

void Debug::checkGLError()
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
	Debug::log(Debug::Area::ClientRendererArea) << "GL error code: " << error << " STRING:" << gluErrorString(error);
        assert(0);
    }
}

void Debug::checkGLErrorSafe()
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
	Debug::log(Debug::Area::ClientRendererArea) << "GL error code: " << error << " STRING:" << gluErrorString(error);
    }
}

#ifdef GLEW_KHR_debug

static void oreGLDebugCallback(unsigned int source, unsigned int type,
                               unsigned int id, unsigned int severity,
                               int length, const char* message, void* userParam);

void Debug::registerGLDebugCallback()
{
    glDebugMessageCallback(&oreGLDebugCallback, 0);
}

//COLOR HOWTO: http://www.ibm.com/developerworks/linux/library/l-tip-prompt/ , only for linux..obviously
void oreGLDebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
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

    Debug::log(Debug::Area::ClientRendererArea) << "\nOpenGL Error Report: " << sourceString << typeString << idString << severityString << messageString << "\n";
}
#endif

////////////////////////////////////////////////////////////////// LogStream class ///////////////////////////////////////////////////////////////////////////

LogStream::LogStream(Debug::Area area) : m_area(area)
{
}

LogStream::~LogStream()
{
    std::string areaString;

    areaString.append("\e[36;40m");

    if (!Settings::instance()->isDebugAreaEnabled(m_area)) {
        return;
    }

    switch (m_area) {
    case Debug::Area::ClientRendererArea:
        areaString.append("[ClientRendererArea]");
        break;

    case Debug::Area::TileRendererArea:
        areaString.append("[TileRendererArea]");
        break;

    case Debug::Area::SpriteSheetRendererArea:
        areaString.append("[SpriteSheetRendererArea]");
        break;

    case Debug::Area::LightingRendererArea:
        areaString.append("[LightingRendererArea]");
        break;

    case Debug::Area::PhysicsArea:
        areaString.append("[PhysicsArea]");
        break;

    case Debug::Area::AudioArea:
        areaString.append("[AudioArea]");
        break;

    case Debug::Area::AudioLoaderArea:
        areaString.append("[AudioLoaderArea]");
        break;

    case Debug::Area::GUILoggerArea:
        areaString.append("[GUILoggerArea]");
        break;

    case Debug::Area::ShadersArea:
        areaString.append("[ShadersArea]");
        break;

    case Debug::Area::NetworkClientInitialArea:
        areaString.append("[NetworkClientInitialArea]");
        break;

    case Debug::Area::NetworkServerInitialArea:
        areaString.append("[NetworkServerInitialArea]");
        break;

    case Debug::Area::NetworkClientContinuousArea:
        areaString.append("[NetworkClientContinuousArea]");
        break;

    case Debug::Area::NetworkServerContinuousArea:
        areaString.append("[NetworkServerContinuousArea]");
        break;

    case Debug::Area::ClientInventoryArea:
        areaString.append("[ClientInventoryArea]");
        break;

    case Debug::Area::ServerInventoryArea:
        areaString.append("[ServerInventoryArea]");
        break;

    case Debug::Area::ServerEntityLogicArea:
        areaString.append("[ServerEntityLogicArea]");
        break;

    case Debug::Area::ImageLoaderArea:
        areaString.append("[ImageLoaderArea]");
        break;

    case Debug::Area::WorldGeneratorArea:
        areaString.append("[WorldGeneratorArea]");
        break;

    case Debug::Area::WorldLoaderArea:
        areaString.append("[WorldLoaderArea]");
        break;

    case Debug::Area::ClientEntityCreationArea:
        areaString.append("[ClientEntityCreationArea]");
        break;

    case Debug::Area::ServerEntityCreationArea:
        areaString.append("[ServerEntityCreationArea]");
        break;

    case Debug::Area::SettingsArea:
        areaString.append("[SettingsArea]");
        break;

    case Debug::Area::StartupArea:
        areaString.append("[StartupArea]");
        break;

    case Debug::Area::ImportantArea:
        areaString.append("[ImportantArea]");
        break;
    }

    areaString.append("\e[36;40m\e[0m");

    std::string msg = str();

    std::string preFormatter;

    preFormatter.append("\e[34;40m");
    preFormatter.append("\n====== ");
    preFormatter.append("\e[34;40m\e[0m");

    std::string postFormatter;
    postFormatter.append("\e[34;40m");
    postFormatter.append(" |========\n");
    postFormatter.append("\e[34;40m\e[0m");

    std::cout << preFormatter << areaString << " | " << str() << postFormatter;
}

LogStream::LogStream(const LogStream& stream): std::stringstream(stream.str())
{
    m_area = stream.m_area;
}
