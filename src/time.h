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

#ifndef TIME_H
#define TIME_H

#include <string>
#include <assert.h>

class Time
{
public:
    Time();
    ~Time();

    /**
     * Called once per update() of the World, increments
     * the world time according to a constant value
     */
    void tick();

    void setTime(uint8_t hour, uint8_t minute) { setHour(hour); setMinute(minute); }

    void setHour(uint8_t hour) { assert(hour <= 23 && hour > 0); m_hour = hour; }

    void setMinute(uint8_t minute) { assert(minute <= 59 && minute >= 0); m_minute = minute; }

    void setSecond(uint8_t second) { assert(second <= 59 && second >= 0); m_second = second; }

    uint8_t currentHour() { return m_hour; }
    uint8_t currentMinute() { return m_minute; }
    uint8_t currentSecond() { return m_second; }

    /**
     * Returns the current time as a formatted string.
     * e.g. 12:45, 23:59, 00:00. 24-hour clock.
     */
    std::string toString();

private:

    //sunrise should be ~07:00, sunset 19:00
    uint8_t m_hour = 0;
    uint8_t m_minute = 0;
    uint8_t m_second = 0;
};
#endif
