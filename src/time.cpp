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

#include "time.h"

#include <sstream>

Time::Time()
{
}

Time::~Time()
{
}

void Time::tick()
{
    uint8_t interval = 1;
//FIXME: SECOND, set interval to 0 too
    m_minute += interval;

    if (m_second >= 60) {
        ++m_minute;
        m_second -= 60;
    }

    if (m_minute >= 60) {
        ++m_hour;
        m_minute -= 60;
    }

    if (m_hour >= 24) {
        m_hour -= 24;
    }

    assert(m_minute <= 59 && m_second <= 59);
    assert(m_hour <= 23);
}

std::string Time::toString() const
{
    std::stringstream ss;
    ss << short(m_hour) << ":";

    //format it so it's native looking wrt single digit minutes
    if (m_minute < 10) {
        ss << "0";
    }

    ss << short(m_minute) << ":";
    ss << short(m_second);
    return ss.str();
}
