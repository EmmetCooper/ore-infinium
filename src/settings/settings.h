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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstdint>
#include <string>

class Settings
{
public:
    static Settings* instance();
    virtual ~Settings();

public:
    int screenResolutionWidth = 1024;
    int screenResolutionHeight = 768;

    //HACK? Question mark question mark.
    int windowWidth = 1024;//screenResolutionWidth;
    int windowHeight = 768;//screenResolutionHeight;

    int debugRendererFlags;

    enum StartupFlags {
        DebugLoggingStartupFlag = 1 << 0,
        WorldViewerStartupFlag = 1 << 1,
        NoTimeoutStartupFlag = 1 << 2,

        //debug, immediately hosts and joins localhost session
        PlayNowStartupFlag = 1 << 3,

        NoSkyRendererStartupFlag = 1 << 4,
        FullDebugStartupFlag = 1 << 5
    };

    /**
     * Enable the provided startup flag
     */
    void setStartupFlag(StartupFlags flag) {
        m_startupFlags |= flag;
    }

    uint32_t startupFlags() const {
        return m_startupFlags;
    }
    void setDebugAreaEnabled(int area, bool enable) {
        if (enable) {
            debugAreas |= area;
        } else {
            debugAreas &= ~area;
        }
    }
    bool isDebugAreaEnabled(int area) {
        return (debugAreas & area) != 0;
    }

private:
    Settings();
    Settings(const Settings& tm) {};
    Settings& operator=(const Settings& tm);

    static Settings* s_instance;

    uint32_t m_startupFlags;
    int debugAreas;
};

#endif
