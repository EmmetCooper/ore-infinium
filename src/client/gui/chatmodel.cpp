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

#include "chatmodel.h"

#include "src/client/client.h"

#include "src/debug.h"

#include <QVariant>

ChatModel::ChatModel(QObject* parent)
: QObject(parent)
{
    setObjectName("chatModel");

    //FIXME: find a better average value to prevent reallocs
    m_chatText.reserve(8000);
    m_chatText = "";
}

ChatModel::~ChatModel()
{
}

void ChatModel::addChatLine(const QString& timestamp, const QString& playerName, const QString& line, ChatModel::ChatSender chatSender)
{
    int row = m_chatLines.size();

    ChatLine chat(timestamp, playerName, line, chatSender);

    m_chatLines.append(chat);

    m_chatText += "timestamp: " + chat.timeStamp + " name: " + chat.playerName + " line: " + chat.chatText + '\n';
    emit chatTextChanged();
}
